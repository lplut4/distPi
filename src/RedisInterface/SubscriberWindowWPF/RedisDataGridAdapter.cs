﻿using System;
using System.Collections.Generic;
using ServiceStack.Redis;
using System.Windows.Controls;
using System.Windows.Data;
using System.Threading;

namespace SubscriberWindowWPF
{
    class RedisDataGridAdapter
    { 
        private          MainWindow m_mainWindow;
        private readonly string     m_redisHost;
        private          DataGrid   m_dataGrid;

        public RedisDataGridAdapter( MainWindow mainWindow, string redisHost, DataGrid dataGrid )
        {
            m_mainWindow = mainWindow;
            m_redisHost  = redisHost;
            m_dataGrid   = dataGrid;
        }

        public void start( List<Type> dataToSubscribeTo )
        {
            var channelsToSubscribe = new List<string>();

            channelsToSubscribe.Add("Node Info");
            channelsToSubscribe.Add("channel-1");
            channelsToSubscribe.Add("channel-2");
            channelsToSubscribe.Add("channel-3");
            channelsToSubscribe.Add("channel-4");

            foreach ( var data in dataToSubscribeTo )
            {
                channelsToSubscribe.Add(data.FullName);
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
            m_dataGrid.Items.Add(new Item() { LocalTime = timeStamp, Channel = "Connecting to ", Data = m_redisHost });

            new Thread(() =>
            {
                var clientsManager = new PooledRedisClientManager(m_redisHost);
                var redisPubSub = new RedisPubSubServer( clientsManager, channelsToSubscribe.ToArray() )
                {
                    OnMessage = (channel, msg) => recordMessage(msg, channel)
                }
                .Start();
                m_mainWindow.Dispatcher.Invoke((Action)(() =>
                {
                    m_dataGrid.Items.Clear();
                }));
            }).Start();
        }

        private void recordMessage(string msg, string channel)
        {
            m_mainWindow.Dispatcher.Invoke((Action)(() =>
            {
                var timeStamp = DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss:ffff");
                m_dataGrid.Items.Add(new Item() { LocalTime = timeStamp, Channel = channel, Data = msg });
            }));
        }

    }
}
