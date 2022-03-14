using Dll_Calcul;
using System;
using System.Globalization;
using System.Windows;
using System.ComponentModel;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Collections.Generic;
using System.Linq;
using OxyPlot;
using OxyPlot.Axes;
using OxyPlot.Series;

namespace PriceurXAML
{
    /// <summary>
    /// Logique d'interaction pour Input.xaml
    /// </summary>
    public partial class InputPutAS : Window
    {
        public InputPutAS()
        {
            InitializeComponent();
        }

        string Spot = "";
        string Strike = "";
        string Taux = "";
        string Vol = "";
        string Mat = "";
        Utilitaire cal = new Utilitaire();
        private ModelView.ModelView viewModel;
        private void Price_Click(object sender, RoutedEventArgs e)
        {
            string err = "";
            Spot = SpotInp.Text;
            Strike = StrikeInp.Text;
            Taux = TauxInp.Text;
            Vol = VolInp.Text;
            Mat = MatInp.Text;
            if (cal.CheckInput("Spot",Spot) !="")
            {
                string caption = "Erreur détécté";
                MessageBoxButtons Ok = MessageBoxButtons.OK;
                DialogResult result;
                err = "La valeur pour 'Spot' n'est pas correct. \n";
                result = System.Windows.Forms.MessageBox.Show(err + cal.CheckInput("Spot", Spot), caption, Ok);
            }
            else if (cal.CheckInput("Strike",Strike) !="")
            {
                string caption = "Erreur détécté";
                MessageBoxButtons Ok = MessageBoxButtons.OK;
                DialogResult result;
                err = "La valeur pour 'Strike' n'est pas correct. \n";
                result = System.Windows.Forms.MessageBox.Show(err + cal.CheckInput("Strike", Strike), caption, Ok);
            }
            else if (cal.CheckInput("Taux", Taux) != "")
            {
                string caption = "Erreur détécté";
                MessageBoxButtons Ok = MessageBoxButtons.OK;
                DialogResult result;
                err = "La valeur pour 'taux' n'est pas correct. \n";
                result = System.Windows.Forms.MessageBox.Show(err + cal.CheckInput("Taux", Taux), caption, Ok);
            }
            else if (cal.CheckInput("Vol", Vol) != "")
            {
                string caption = "Erreur détécté";
                MessageBoxButtons Ok = MessageBoxButtons.OK;
                DialogResult result;
                err = "La valeur pour 'Volatility' n'est pas correct. \n";
                result = System.Windows.Forms.MessageBox.Show(err + cal.CheckInput("Vol", Vol), caption, Ok);
            }
            else if (cal.CheckInput("Mat", Mat) != "")
            {
                string caption = "Erreur détécté";
                MessageBoxButtons Ok = MessageBoxButtons.OK;
                DialogResult result;
                err = "La valeur pour 'Maturité' n'est pas correct. \n";
                result = System.Windows.Forms.MessageBox.Show(err + cal.CheckInput("Mat", Mat), caption, Ok);
            }
            else
            {
                DisplayDataCallEU();
            }

        }

        private async void DisplayDataCallEU()
        {
            PriceInp.Visibility = Visibility.Hidden;
            Spot = SpotInp.Text;
            Strike = StrikeInp.Text;
            Taux = TauxInp.Text;
            Vol = VolInp.Text;
            Mat = MatInp.Text;

            double S = double.Parse(Spot, CultureInfo.InvariantCulture);
            double K = double.Parse(Strike, CultureInfo.InvariantCulture);
            double r = double.Parse(Taux, CultureInfo.InvariantCulture);
            double v = double.Parse(Vol, CultureInfo.InvariantCulture);
            double T = double.Parse(Mat, CultureInfo.InvariantCulture);
            
            decimal Prime = 0.0m;

            PbStatus.Visibility = Visibility.Visible;
            PbCal.Visibility = Visibility.Visible;
            PbStatus.Value = 0;
            var points = new List<Simu>();
            await Task.Run(() =>
            {
                for (int i = 0; i < 100000; i++)
                {
                    if (i % 1000 == 0)
                    {
                        this.Dispatcher.Invoke(() =>
                        {
                            PbStatus.Value = (int)i / 1000;
                        });
                        //decimal[] traji = cal.MBG(S, r, v, T);
                        //for (int s = 0; s < traji.Length - 1; s++)
                        //{
                        //    points.Add(new Simu() { id = i, pas = s, Value = traji[s] });
                        //}
                        //decimal somme1 = (decimal)Math.Max((double)cal.SommeTraj(traji) / 252.0 - K, 0.0);
                        //decimal somme2 = (decimal)Math.Max(Math.Exp((double)cal.LogSommeTraj(traji)/252 -K),0.0);
                        //Prime = Prime + somme1 - somme2;
                    }
                    decimal[] traj = cal.MBG(S, r, v, T);
                    decimal somme1 = (decimal)Math.Max((double)cal.SommeTraj(traj) / 252.0 - K, 0.0);
                    decimal somme2 = (decimal)Math.Max(Math.Exp((double)cal.LogSommeTraj(traj) / 252 - K), 0.0);
                    Prime = Prime + somme1 - somme2;
                }
                var AllData = points.GroupBy(m => m.id).ToList();
                viewModel = new ModelView.ModelView();
                foreach (var data in AllData)
                {
                    var lines = new LineSeries
                    {
                        StrokeThickness = 2,
                        MarkerSize = 3,
                        CanTrackerInterpolatePoints = false,
                    };
                    data.ToList().ForEach(d => lines.Points.Add(new DataPoint(d.pas, Convert.ToDouble(d.Value))));
                    viewModel.AddLines(lines);
                }
            });

            DataContext = viewModel;

            PbStatus.Visibility = Visibility.Collapsed;
            PbCal.Visibility = Visibility.Collapsed;
            PriceInp.Visibility = Visibility.Visible;
            Prime = Prime / 100000m + cal.Ez(S, r, v, T, K, 1.0);
            if (Prime<0.0m)
            {
                Prime = 0.0m;
            }
            Prime = Math.Round(Prime, 6);
            MCInp.Text = Prime.ToString();
            MCInp.Visibility = Visibility.Visible;
            MC.Visibility = Visibility.Visible;
        }

        private void Window_PreviewKeyDown(object sender, System.Windows.Input.KeyEventArgs e)
        {
            if (e.Key == System.Windows.Input.Key.Enter)
            {
                Price_Click(sender, e);
            }
        }
        public class Simu
        {
            public decimal Value { get; set; }
            public int id { get; set; }
            public int pas { get; set; }
        }
    }
}

