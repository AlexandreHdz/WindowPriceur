using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Xps.Packaging;
using System.IO;

namespace PriceurXAML
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        private void CallEU_Click(object sender, RoutedEventArgs e)
        {
            InputCallEU p = new InputCallEU();
            p.Show();
        }
        private void GetYahooData(object sender, RoutedEventArgs e)
        {
            YahooWind yahoo = new YahooWind();
            yahoo.Show();
        }

        private void PutEU_Click(object sender, RoutedEventArgs e)
        {
            InputPutEU p = new InputPutEU();
            p.Show();
        }

        private void CallAS_Click(object sender, RoutedEventArgs e)
        {
            InputCallAS p = new InputCallAS();
            p.Show();
        }

        private void PutAS_Click(object sender, RoutedEventArgs e)
        {
            InputPutAS p = new InputPutAS();
            p.Show();
        }

        private void CallKO_Click(object sender, RoutedEventArgs e)
        {

        }

        private void PutKO_Click(object sender, RoutedEventArgs e)
        {

        }

        private void GetHelp(object sender, RoutedEventArgs e)
        {
            Help p = new Help();
            p.Show();
        }
    }
}
