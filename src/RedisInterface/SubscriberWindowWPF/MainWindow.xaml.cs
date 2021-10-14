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

        RedisClient m_publisher;

        public MainWindow()
        {
            InitializeComponent();

            m_publisher = new RedisClient("raspberrypi");

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
                channelsToSubscribe.Add("channel-2");
                channelsToSubscribe.Add("channel-3");
                channelsToSubscribe.Add("channel-4");
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

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            m_publisher.PublishMessage("channel-1", "pushed button 1");
        }

        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            m_publisher.PublishMessage("channel-2", "pushed button 2");

        }

        private void m_dataGrid_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            // Example of inspecting the selected cell of the data column
            DataGrid dataGrid = sender as DataGrid;
            DataGridRow row = (DataGridRow)dataGrid.ItemContainerGenerator.ContainerFromIndex(dataGrid.SelectedIndex);
            DataGridCell RowColumn = dataGrid.Columns[2].GetCellContent(row).Parent as DataGridCell;
            string CellValue = ((TextBlock)RowColumn.Content).Text;
        }
    }
}
