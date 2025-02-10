using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Runtime.InteropServices;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Forms;

namespace ZilophiXGUITool
{
    public partial class MainWindow : Window
    {
        // Private fields.
        private readonly List<string> targetFiles;
        private readonly BackgroundWorker worker;
        private string convOutDir;
        private ushort encBlockSize;
        private bool encUseMidSideStereo;
        private byte encSSLMSTaps;

        // Constructor
        public MainWindow()
        {
            this.targetFiles = new List<string>();
            this.worker = new BackgroundWorker();
            this.worker.WorkerReportsProgress = true;
            this.worker.DoWork += OnProcess;
            this.worker.ProgressChanged += OnProcessProgressChanged;
            this.worker.RunWorkerCompleted += OnProcessCompleted;

            InitializeComponent();
        }

        [DllImport("zilophixenc.dll", CharSet = CharSet.Unicode)]
        private static extern void ZpXEncodeFileW(string input, string output, ushort blockSize, bool useMidSideStereo, byte filterTaps);

        [DllImport("zilophixdec.dll", CharSet = CharSet.Unicode)]
        private static extern void ZpXDecodeFileW(string input, string output);

        /// <summary>
        /// Conversion
        /// </summary>
        /// <param name="input"></param>
        private void Process(string input)
        {
            string extension = Path.GetExtension(input).ToLower();

            if (!Directory.Exists(this.convOutDir))
            {
                Directory.CreateDirectory(this.convOutDir);
            }

            if (extension == ".wav")
            {
                string output = $"{this.convOutDir}\\{Path.GetFileNameWithoutExtension(input)}.zpx";

                ZpXEncodeFileW(input, output, this.encBlockSize, this.encUseMidSideStereo, this.encSSLMSTaps);
            }
            else if (extension == ".zpx")
            {
                string output = $"{this.convOutDir}\\{Path.GetFileNameWithoutExtension(input)}.wav";

                ZpXDecodeFileW(input, output);
            }
        }

        /// <summary>
        /// Start background conversion task.
        /// </summary>
        private void StartProcess()
        {
            this.convOutDir = this.OutputDirectoryPathTextBox.Text;
            this.encBlockSize = Convert.ToUInt16(((ComboBoxItem)this.BlockSizeComboBox.SelectedItem).Tag);
            this.encUseMidSideStereo = this.UseMidSideConvertingCheckBox.IsChecked.Value;
            this.encSSLMSTaps = (byte)this.SSLMSFilterTapsTrack.Value;

            this.BlockSizeComboBox.IsEnabled = false;
            this.UseMidSideConvertingCheckBox.IsEnabled = false;
            this.SSLMSFilterTapsTrack.IsEnabled = false;
            this.AddFileButton.IsEnabled = false;
            this.RemoveFileButton.IsEnabled = false;
            this.StartProcessButton.IsEnabled = false;
            this.OutputDirectoryPathTextBox.IsEnabled = false;
            this.SelectOutputDirectoryButton.IsEnabled = false;
            this.ProcessProgressBar.Value = 0;

            this.worker.RunWorkerAsync();
        }

        /// <summary>
        /// Update file list.
        /// </summary>
        private void UpdateFileList()
        {
            this.ConvertItemsListBox.Items.Clear();

            foreach (var path in this.targetFiles)
            {
                var item = new ListBoxItem();
                item.Content = path;
                item.HorizontalContentAlignment = System.Windows.HorizontalAlignment.Stretch;
                item.VerticalContentAlignment = VerticalAlignment.Stretch;

                this.ConvertItemsListBox.Items.Add(item);
            }

            this.StartProcessButton.IsEnabled = this.ConvertItemsListBox.Items.Count > 0;
        }

        private void OnProcessCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            this.BlockSizeComboBox.IsEnabled = true;
            this.UseMidSideConvertingCheckBox.IsEnabled = true;
            this.SSLMSFilterTapsTrack.IsEnabled = true;
            this.AddFileButton.IsEnabled = true;
            this.RemoveFileButton.IsEnabled = true;
            this.StartProcessButton.IsEnabled = true;
            this.OutputDirectoryPathTextBox.IsEnabled = true;
            this.SelectOutputDirectoryButton.IsEnabled = true;

            System.Windows.MessageBox.Show("Conversion completed successfully!", "Completed", MessageBoxButton.OK, MessageBoxImage.Information);
        }

        private void OnProcessProgressChanged(object sender, ProgressChangedEventArgs e)
        {
            this.ProcessProgressBar.Value = e.ProgressPercentage;
        }

        private void OnProcess(object sender, DoWorkEventArgs e)
        {
            for (int i = 0; i < this.targetFiles.Count; ++i)
            {
                Process(this.targetFiles[i]);

                // 進捗を報告
                int percentage = (i + 1) * 100 / this.targetFiles.Count;
                this.worker.ReportProgress(percentage);
            }
        }

        private void ConvertItemsListBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            this.RemoveFileButton.IsEnabled = this.ConvertItemsListBox.SelectedItems.Count != 0;
        }

        private void AddFileButton_Click(object sender, RoutedEventArgs e)
        {
            var dialog = new Microsoft.Win32.OpenFileDialog();
            dialog.Filter = "All supported files(*.wav;*.zpx)|*.wav;*.zpx|WAV(*.wav)|*.wav|ZilophiX(*.zpx)|*.zpx";
            dialog.Multiselect = true;

            if (dialog.ShowDialog().Value)
            {
                foreach (var path in dialog.FileNames)
                {
                    if (!this.targetFiles.Contains(path))
                    {
                        this.targetFiles.Add(path);
                    }
                }
            }

            UpdateFileList();
        }

        private void RemoveFileButton_Click(object sender, RoutedEventArgs e)
        {
            var item = (ListBoxItem)this.ConvertItemsListBox.SelectedItem;
            var path = item.Content.ToString();

            if (this.targetFiles.Contains(path))
            {
                this.targetFiles.Remove(path);
            }

            UpdateFileList();
        }

        private void SSLMSFilterTapsTrack_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            if (!this.IsLoaded)
            {
                return;
            }

            this.SSLMSFilterTapsLabel.Text = this.SSLMSFilterTapsTrack.Value.ToString();
        }

        private void SelectOutputDirectoryButton_Click(object sender, RoutedEventArgs e)
        {
            using (var dialog = new FolderBrowserDialog())
            {
                if (dialog.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                {
                    this.OutputDirectoryPathTextBox.Text = dialog.SelectedPath;
                }
            }   
        }

        private void StartProcessButton_Click(object sender, RoutedEventArgs e)
        {
            StartProcess();
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            this.BlockSizeComboBox.SelectedIndex = 2;
            this.RemoveFileButton.IsEnabled = this.ConvertItemsListBox.SelectedItems.Count != 0;
            this.StartProcessButton.IsEnabled = this.ConvertItemsListBox.Items.Count > 0;
        }
    }
}
