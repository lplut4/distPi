using System.Windows;
using System.Windows.Controls;
using ServiceStack.Redis;

namespace SubscriberWindowWPF
{
    public partial class MainWindow : Window
    {
        RedisClient          m_redisClient;
        RedisDataGridAdapter m_redisGridAdapter;

        public MainWindow()
        {
            InitializeComponent();

            m_redisClient = new RedisClient("raspberrypi");
            m_redisGridAdapter = new RedisDataGridAdapter(this, "raspberrypi", m_dataGrid);

            m_redisGridAdapter.start();
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            m_redisClient.PublishMessage("channel-1", "pushed button 1");
        }

        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            m_redisClient.PublishMessage("channel-2", "pushed button 2");
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
