using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Windows;

namespace SubscriberWindowWPF
{
    public partial class App : Application
    {
        public static ReadOnlyCollection<string> GetApplicationArgs()
        {
            return new ReadOnlyCollection<string>(s_applicationArgs);
        }

        private static List<string> s_applicationArgs = new List<string>();

        private void Application_Startup(object sender,StartupEventArgs e)
        {
            var args = e.Args;
            if (args != null && args.Count() > 0)
            {
                foreach (var arg in args)
                {
                    s_applicationArgs.Add(arg);
                }
            }
        }
    }
}
