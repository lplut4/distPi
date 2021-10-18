using System;
using System.Collections.Generic;
using StackExchange.Redis;
using System.Windows.Controls;
using DataModel;

namespace SubscriberWindowWPF
{
    class RedisDataGridAdapter
    { 
        private MainWindow            m_mainWindow;
        private readonly string       m_redisHost;
        private ConnectionMultiplexer m_redisClient;
        private DataGrid              m_dataGrid;
        private bool                  m_started;

        public RedisDataGridAdapter( MainWindow mainWindow, string redisHost, DataGrid dataGrid )
        {
            m_mainWindow  = mainWindow;
            m_redisHost   = redisHost;
            m_redisClient = null;
            m_dataGrid    = dataGrid;
            m_started     = false;
        }

        public void start( List<Type> dataToSubscribeTo )
        {
            if ( m_started )
            {
                return;
            }

            startAdapter( dataToSubscribeTo );
            m_mainWindow.setRedisClient(m_redisClient);

            m_started = true;
        }

        private void startAdapter( List<Type> dataToSubscribeTo )
        {
            ConfigurationOptions options = new ConfigurationOptions();

            // TODO - Need to figure out how to deal with connection drop-outs

            // Continue trying to connect until a connection is established
            options.ConnectRetry = int.MaxValue;  
            options.EndPoints.Add(m_redisHost);

            m_redisClient = ConnectionMultiplexer.Connect(options);

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

            foreach (var channel in channelsToSubscribe)
            {
                var sub = m_redisClient.GetSubscriber();
                sub.Subscribe(channel).OnMessage(channelMessage => {
                    recordByteMessage(channelMessage.Message, channel);
                });
            }
        }

        private void recordByteMessage(byte[] buffer, string channel)
        {
            m_mainWindow.Dispatcher.Invoke((Action)(() =>
            {
                var timeStamp = DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss:ffff");

                var dataType = ReflectiveDataModelCollection.GetSerializableType(channel);

                if (dataType == null)
                {
                    var str = System.Text.Encoding.Default.GetString(buffer);
                    recordMessage(str, channel);
                    return;
                }

                try
                {
                    var data = ReflectiveDataModelCollection.Deserialize(dataType, buffer);
                    var decodedData = ReflectiveDataModelCollection.DecodeMessage(data);
                    m_dataGrid.Items.Add(new GridItem() { LocalTime = timeStamp, Channel = channel, Data = decodedData });
                }
                catch (Google.Protobuf.InvalidProtocolBufferException)
                {
                    // Control message?  Should we ignore it?
                }
            }));
        }

        private void recordMessage(string msg, string channel)
        {
            m_mainWindow.Dispatcher.Invoke((Action)(() =>
            {
                var timeStamp = DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss:ffff");
                m_dataGrid.Items.Add(new GridItem() { LocalTime = timeStamp, Channel = channel, Data = msg });
            }));
        }
    }
}
