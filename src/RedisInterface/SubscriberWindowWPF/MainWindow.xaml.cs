using System;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using DataModel;
using StackExchange.Redis;

namespace SubscriberWindowWPF
{
    public partial class MainWindow : Window
    {
        ConnectionMultiplexer m_redisClient;
        RedisDataGridAdapter  m_redisGridAdapter;

        public MainWindow()
        {
            InitializeComponent();

            var redisHost = "localhost";

            var args = App.GetApplicationArgs();
            if (args.Count > 0)
            {
                redisHost = args[0];
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
            m_dataGrid.Items.Add(new GridItem() { LocalTime = timeStamp, Channel = "Connecting to ", Data = redisHost });

            m_redisGridAdapter = new RedisDataGridAdapter(this, redisHost, m_dataGrid);

            new Thread(() =>
            {
                var dataModelLibrary = ReflectiveDataModelCollection.GetSerializableTypes();
                m_redisGridAdapter.start(dataModelLibrary);
            }).Start();
        }

        public void setRedisClient( ConnectionMultiplexer redisClient )
        {
            m_redisClient = redisClient;
            this.Dispatcher.Invoke((Action)(() =>
            {
                m_dataGrid.Items.Clear();
            }));
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            if (m_redisClient == null)
            {
                return;
            }

            var subscriber = m_redisClient.GetSubscriber();
            subscriber.Publish("channel-1", "pushed button 1");
        }

        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            if (m_redisClient == null)
            {
                return;
            }

            var time = new TimeSpec();
            time.TvNsec = 1;
            time.TvSec = 1000;
            byte[] buffer = ReflectiveDataModelCollection.Serialize(time);

            var subscriber = m_redisClient.GetSubscriber();
            subscriber.Publish(time.GetType().FullName, buffer);
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
