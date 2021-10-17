using System.Windows;
using System.Windows.Controls;
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
            var redisHost = "raspberrypi";

            var args = App.GetApplicationArgs();
            if (args.Count > 0)
            {
                redisHost = args[0];
            }

            InitializeComponent();

            // TODO - Replace this with a ConnectAsync call to prevent the UI from waiting on it
            m_redisClient = ConnectionMultiplexer.Connect(redisHost);

            var dataModelLibrary = ReflectiveDataModelCollection.GetSerializableTypes();

            m_redisGridAdapter = new RedisDataGridAdapter(this, m_redisClient, m_dataGrid);
            m_redisGridAdapter.start( dataModelLibrary );
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            var subscriber = m_redisClient.GetSubscriber();
            subscriber.Publish("channel-1", "pushed button 1");
        }

        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
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
