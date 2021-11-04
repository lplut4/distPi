using System;
using System.Collections.Generic;
using StackExchange.Redis;
using System.Windows.Controls;
using DataModel;
using System.Windows.Media;
using System.Text;

namespace ChronologicalMessageWindow
{
    public class RedisDataGridAdapter : IDisposable
    {
        private SolidColorBrush WHITE  = new SolidColorBrush(Colors.White);
        private SolidColorBrush GREEN  = new SolidColorBrush(Colors.LightGreen);
        private SolidColorBrush YELLOW = new SolidColorBrush(Colors.Yellow);
        private SolidColorBrush RED    = new SolidColorBrush(Colors.Red);

        private MainWindow            m_mainWindow;
        private readonly string       m_redisHost;
        private ConnectionMultiplexer m_redisClient;
        private bool                  m_started;
        private bool                  m_terminateApp;

        public RedisDataGridAdapter( MainWindow mainWindow, string redisHost, DataGrid dataGrid )
        {
            m_mainWindow  = mainWindow;
            m_redisHost   = redisHost;
            m_redisClient = null;
            m_started     = false;
        }

        public void Dispose()
        {
            m_terminateApp = true;
            if (m_redisClient != null)
            {
                m_redisClient.Close(true);
                m_redisClient.Dispose();
            }
        }

        public void Start( List<Type> dataToSubscribeTo )
        {
            if ( m_started )
            {
                return;
            }

            m_mainWindow.SetStatus("Connecting to: " + m_redisHost, WHITE);

            StartAdapter( dataToSubscribeTo );
            m_mainWindow.SetRedisClient(m_redisClient);

            m_started = true;
        }

        private void StartAdapter( List<Type> dataToSubscribeTo )
        {
            ConfigurationOptions options = new ConfigurationOptions();

            options.EndPoints.Add( m_redisHost );
            options.ConnectRetry = 1;
            options.ConnectTimeout = 1000;

            while (m_redisClient == null && !m_terminateApp)
            {
                try
                {
                    m_redisClient = ConnectionMultiplexer.Connect(options);
                    m_redisClient.ConnectionFailed   += ConnectionFailed;
                    m_redisClient.ConnectionRestored += ConnectionRestored;
                }
                catch (RedisConnectionException)
                {
                    // Try again rather than waiting indefinitely.
                    // Forever sitting within the connect call prevents the App from terminating when the user closes the main window
                }
            }

            if ( m_terminateApp || m_redisClient == null )
            {
                return;
            }

            var channelsToSubscribe = new List<string>();

            channelsToSubscribe.Add("Node Info");
            channelsToSubscribe.Add("channel-1");
            channelsToSubscribe.Add("channel-2");
            channelsToSubscribe.Add("channel-3");
            channelsToSubscribe.Add("channel-4");

            foreach (var data in dataToSubscribeTo)
            {
                channelsToSubscribe.Add(data.FullName);
            }

            var subscriptions = new StringBuilder();
            foreach (var channel in channelsToSubscribe)
            {
                subscriptions.Append(channel).Append(", ");
                var sub = m_redisClient.GetSubscriber();
                sub.Subscribe( channel ).OnMessage( channelMessage => { RecordByteMessage( channelMessage.Message, channel ); } );
            }

            m_mainWindow.SetStatus("Connected to: " + m_redisHost, GREEN);
            m_mainWindow.WriteToGrid("EVENT", "Subscribed to: " + subscriptions.ToString().TrimEnd( new char[] { ' ', ',' } ) );
        }

        private void ConnectionFailed(object sender, ConnectionFailedEventArgs e)
        {
            m_mainWindow.SetStatus("Dropped connection to: " + e.EndPoint, RED);
        }

        private void ConnectionRestored(object sender, ConnectionFailedEventArgs e)
        {
            m_mainWindow.SetStatus("Reconnected to: " + e.EndPoint, YELLOW);
        }

        private void RecordByteMessage(byte[] buffer, string channel)
        {          
            var dataType = ReflectiveDataModelCollection.GetSerializableType(channel);

            if (dataType == null)
            {
                var str = Encoding.Default.GetString(buffer);
                m_mainWindow.WriteToGrid(channel, str);
                return;
            }

            try
            {
                var data = ReflectiveDataModelCollection.Deserialize(dataType, buffer);
                var jsonString = Google.Protobuf.JsonFormatter.Default.Format(data);
                m_mainWindow.WriteToGrid(channel, jsonString);
            }
            catch (Google.Protobuf.InvalidProtocolBufferException)
            {
                var str = Encoding.Default.GetString(buffer);
                m_mainWindow.WriteToGrid(channel, str);
            }
        }
    }
}