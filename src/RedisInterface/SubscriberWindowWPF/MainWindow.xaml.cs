using System;
using System.Collections.Generic;
using System.Threading;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using ServiceStack.Redis;

namespace SubscriberWindowWPF
{
    public partial class MainWindow : Window
    { 

        public MainWindow()
        {
            InitializeComponent();

            var args = Environment.GetCommandLineArgs();

            var redisHost = "raspberrypi";
            var channelsToSubscribe = new List<string>();
            if ( args.Length > 2 )
            {
                redisHost = args[0];
                for (int i = 1; i < args.Length; i++)
                {
                    channelsToSubscribe.Add(args[i]);
                }
            }
            else
            {
                channelsToSubscribe.Add("channel-1");
            }

            {
                var column = new DataGridTextColumn();
                column.Header = "Local Time";
                column.Binding = new Binding("LocalTime");
                m_dataGrid.Columns.Add(column);
            }
            {
                var column = new DataGridTextColumn();
                column.Header = "Channel";
                column.Binding = new Binding("Channel");
                m_dataGrid.Columns.Add(column);
            }
            {
                var column = new DataGridTextColumn();
                column.Header = "Data";
                column.Binding = new Binding("Data");
                m_dataGrid.Columns.Add(column);
            }

            var timeStamp = DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss:ffff");
            m_dataGrid.Items.Add(new Item() { LocalTime = timeStamp, Channel = "Connecting to ", Data = redisHost });

            new Thread(() =>
            {
                var clientsManager = new PooledRedisClientManager( redisHost );
                var redisPubSub = new RedisPubSubServer( clientsManager, channelsToSubscribe.ToArray() )
                {
                    OnMessage = (channel, msg) => recordMessage(msg, channel)
                }
                .Start();
                this.Dispatcher.Invoke((Action)(() =>
                {
                    m_dataGrid.Items.Clear();
                }));
            }).Start();
        }

        private void recordMessage( string msg, string channel )
        {
            this.Dispatcher.Invoke((Action)(() =>
            {
                var timeStamp = DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss:ffff");
                m_dataGrid.Items.Add(new Item() { LocalTime = timeStamp, Channel = channel, Data = msg });
            }));
        }
    }
}
