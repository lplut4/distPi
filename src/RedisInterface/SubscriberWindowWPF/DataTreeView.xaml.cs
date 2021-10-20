using System;
using System.Collections.Generic;
using System.Globalization;
using System.Windows;
using System.Windows.Data;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace SubscriberWindowWPF
{
    // Respectfully adapted from https://stackoverflow.com/questions/23812357/how-to-bind-dynamic-json-into-treeview-wpf/28097883

    public sealed class MethodToValueConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            var methodName = parameter as string;
            if (value == null || methodName == null)
                return null;
            var methodInfo = value.GetType().GetMethod(methodName, new Type[0]);
            if (methodInfo == null)
                return null;
            return methodInfo.Invoke(value, new object[0]);
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotSupportedException(GetType().Name + " can only be used for one way conversion.");
        }
    }

    public partial class DataTreeView : Window
    {
        private readonly string m_title;
        private readonly string m_message;

        public DataTreeView( string timestamp, string channel, string message )
        {
            InitializeComponent();

            m_message = message;
            m_title = channel + " @ " + timestamp;
            this.Title = m_title;

            var token = JToken.Parse(message);

            var children = new List<JToken>();
            if (token != null)
            {
                children.Add(token);
            }

            m_treeView.ItemsSource = null;
            m_treeView.Items.Clear();
            m_treeView.ItemsSource = children;
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            var obj = JsonConvert.DeserializeObject(m_message);
            var formatted = JsonConvert.SerializeObject(obj, Formatting.Indented);
            Clipboard.SetText(m_title + "\n" + formatted);
        }
    }
}
