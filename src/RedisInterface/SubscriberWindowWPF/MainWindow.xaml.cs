using System;
using System.Threading;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Input;
using System.Windows.Media;
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

            m_redisGridAdapter = new RedisDataGridAdapter(this, redisHost, m_dataGrid);

            new Thread(() =>
            {
                var dataModelLibrary = ReflectiveDataModelCollection.GetSerializableTypes();
                m_redisGridAdapter.Start(dataModelLibrary);
            }).Start();
        }

        public void SetRedisClient( ConnectionMultiplexer redisClient )
        {
            m_redisClient = redisClient;
        }

        public void SetStatus( string status, Brush color )
        {
            var statusChanged = false;
            this.Dispatcher.Invoke((Action)(() =>
            {
                statusChanged = !m_statusLabel.Content.ToString().Equals(status);
                m_statusLabel.Content = status;
                m_statusLabel.Visibility = Visibility.Visible;
                m_statusLabel.Foreground = color;
            }));

            if (statusChanged)
            {
                WriteToGrid("EVENT", status);
            }
        }

        public void WriteToGrid(string channel, string data)
        {
            this.Dispatcher.Invoke((Action)(() =>
            {
                var timeStamp = DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss:ffff");
                m_dataGrid.Items.Add(new GridItem() { LocalTime = timeStamp, Channel = channel, Data = data });

                var performAutoScroll = m_autoScrollCheckbox.IsChecked.HasValue ? m_autoScrollCheckbox.IsChecked.Value : false;

                if (performAutoScroll && m_dataGrid.Items.Count > 0)
                {
                    var border = VisualTreeHelper.GetChild(m_dataGrid, 0) as Decorator;
                    if (border != null)
                    {
                        var scroll = border.Child as ScrollViewer;
                        if (scroll != null) scroll.ScrollToEnd();
                    }
                }
            }));
        }

        private void Button1_Click(object sender, RoutedEventArgs e)
        {
            if (m_redisClient == null || !m_redisClient.IsConnected)
            {
                return;
            }

            var subscriber = m_redisClient.GetSubscriber();
            subscriber.Publish("channel-1", "pushed button 1");
        }

        private void Button2_Click(object sender, RoutedEventArgs e)
        {
            if (m_redisClient == null || !m_redisClient.IsConnected)
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

        private void DataGrid_MouseDoubleClick( object sender, MouseEventArgs e )
        { 
            // Example of inspecting the selected cell of the data column
            DataGrid dataGrid = sender as DataGrid;
            DataGridRow row = (DataGridRow)dataGrid.ItemContainerGenerator.ContainerFromIndex(dataGrid.SelectedIndex);
            DataGridCell RowColumn = dataGrid.Columns[2].GetCellContent(row).Parent as DataGridCell;
            string CellValue = ((TextBlock)RowColumn.Content).Text;
        }
    }
}
