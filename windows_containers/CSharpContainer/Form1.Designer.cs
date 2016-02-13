namespace CSharpContainer
{
    partial class CSharpMapContainer
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要修改
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(CSharpMapContainer));
            this.tabControl_map = new System.Windows.Forms.TabControl();
            this.tabPage_map1 = new System.Windows.Forms.TabPage();
            this.listBox_msg = new System.Windows.Forms.ListBox();
            this.label_msg = new System.Windows.Forms.Label();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.connectionsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.getConnectionAddressToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.axqtaxviewer_planetosm1 = new Axqtaxviewer_planetosmLib.Axqtaxviewer_planetosm();
            this.tabPage_map2 = new System.Windows.Forms.TabPage();
            this.axqtaxviewer_planetosm2 = new Axqtaxviewer_planetosmLib.Axqtaxviewer_planetosm();
            this.tabPage_msg = new System.Windows.Forms.TabPage();
            this.performanceToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.heavyTestToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.tabControl_map.SuspendLayout();
            this.tabPage_map1.SuspendLayout();
            this.menuStrip1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.axqtaxviewer_planetosm1)).BeginInit();
            this.tabPage_map2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.axqtaxviewer_planetosm2)).BeginInit();
            this.SuspendLayout();
            // 
            // tabControl_map
            // 
            this.tabControl_map.Controls.Add(this.tabPage_map1);
            this.tabControl_map.Controls.Add(this.tabPage_map2);
            this.tabControl_map.Controls.Add(this.tabPage_msg);
            this.tabControl_map.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControl_map.Location = new System.Drawing.Point(0, 0);
            this.tabControl_map.Name = "tabControl_map";
            this.tabControl_map.SelectedIndex = 0;
            this.tabControl_map.Size = new System.Drawing.Size(912, 562);
            this.tabControl_map.TabIndex = 0;
            // 
            // tabPage_map1
            // 
            this.tabPage_map1.Controls.Add(this.listBox_msg);
            this.tabPage_map1.Controls.Add(this.label_msg);
            this.tabPage_map1.Controls.Add(this.menuStrip1);
            this.tabPage_map1.Controls.Add(this.axqtaxviewer_planetosm1);
            this.tabPage_map1.Location = new System.Drawing.Point(4, 22);
            this.tabPage_map1.Name = "tabPage_map1";
            this.tabPage_map1.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage_map1.Size = new System.Drawing.Size(904, 536);
            this.tabPage_map1.TabIndex = 0;
            this.tabPage_map1.Text = "Map1";
            this.tabPage_map1.UseVisualStyleBackColor = true;
            // 
            // listBox_msg
            // 
            this.listBox_msg.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.listBox_msg.FormattingEnabled = true;
            this.listBox_msg.HorizontalScrollbar = true;
            this.listBox_msg.ItemHeight = 12;
            this.listBox_msg.Location = new System.Drawing.Point(687, 0);
            this.listBox_msg.Name = "listBox_msg";
            this.listBox_msg.Size = new System.Drawing.Size(217, 532);
            this.listBox_msg.TabIndex = 1;
            // 
            // label_msg
            // 
            this.label_msg.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.label_msg.AutoSize = true;
            this.label_msg.Location = new System.Drawing.Point(1, 499);
            this.label_msg.Name = "label_msg";
            this.label_msg.Size = new System.Drawing.Size(41, 12);
            this.label_msg.TabIndex = 1;
            this.label_msg.Text = "label1";
            // 
            // menuStrip1
            // 
            this.menuStrip1.Dock = System.Windows.Forms.DockStyle.None;
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.connectionsToolStripMenuItem,
            this.performanceToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(3, 3);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(283, 25);
            this.menuStrip1.TabIndex = 1;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // connectionsToolStripMenuItem
            // 
            this.connectionsToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.getConnectionAddressToolStripMenuItem});
            this.connectionsToolStripMenuItem.Name = "connectionsToolStripMenuItem";
            this.connectionsToolStripMenuItem.Size = new System.Drawing.Size(89, 21);
            this.connectionsToolStripMenuItem.Text = "connections";
            // 
            // getConnectionAddressToolStripMenuItem
            // 
            this.getConnectionAddressToolStripMenuItem.Name = "getConnectionAddressToolStripMenuItem";
            this.getConnectionAddressToolStripMenuItem.Size = new System.Drawing.Size(213, 22);
            this.getConnectionAddressToolStripMenuItem.Text = "get connection address";
            this.getConnectionAddressToolStripMenuItem.Click += new System.EventHandler(this.getConnectionAddressToolStripMenuItem_Click);
            // 
            // axqtaxviewer_planetosm1
            // 
            this.axqtaxviewer_planetosm1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.axqtaxviewer_planetosm1.Enabled = true;
            this.axqtaxviewer_planetosm1.Location = new System.Drawing.Point(-4, 3);
            this.axqtaxviewer_planetosm1.Name = "axqtaxviewer_planetosm1";
            this.axqtaxviewer_planetosm1.OcxState = ((System.Windows.Forms.AxHost.State)(resources.GetObject("axqtaxviewer_planetosm1.OcxState")));
            this.axqtaxviewer_planetosm1.Size = new System.Drawing.Size(685, 533);
            this.axqtaxviewer_planetosm1.TabIndex = 0;
            this.axqtaxviewer_planetosm1.evt_Message += new Axqtaxviewer_planetosmLib.Iqtaxviewer_planetosmEvents_evt_MessageEventHandler(this.axqtaxviewer_planetosm1_evt_Message);
            // 
            // tabPage_map2
            // 
            this.tabPage_map2.Controls.Add(this.axqtaxviewer_planetosm2);
            this.tabPage_map2.Location = new System.Drawing.Point(4, 22);
            this.tabPage_map2.Name = "tabPage_map2";
            this.tabPage_map2.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage_map2.Size = new System.Drawing.Size(904, 536);
            this.tabPage_map2.TabIndex = 1;
            this.tabPage_map2.Text = "Map2";
            this.tabPage_map2.UseVisualStyleBackColor = true;
            // 
            // axqtaxviewer_planetosm2
            // 
            this.axqtaxviewer_planetosm2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.axqtaxviewer_planetosm2.Enabled = true;
            this.axqtaxviewer_planetosm2.Location = new System.Drawing.Point(3, 3);
            this.axqtaxviewer_planetosm2.Name = "axqtaxviewer_planetosm2";
            this.axqtaxviewer_planetosm2.OcxState = ((System.Windows.Forms.AxHost.State)(resources.GetObject("axqtaxviewer_planetosm2.OcxState")));
            this.axqtaxviewer_planetosm2.Size = new System.Drawing.Size(898, 530);
            this.axqtaxviewer_planetosm2.TabIndex = 0;
            this.axqtaxviewer_planetosm2.evt_Message += new Axqtaxviewer_planetosmLib.Iqtaxviewer_planetosmEvents_evt_MessageEventHandler(this.axqtaxviewer_planetosm2_evt_Message);
            // 
            // tabPage_msg
            // 
            this.tabPage_msg.Location = new System.Drawing.Point(4, 22);
            this.tabPage_msg.Name = "tabPage_msg";
            this.tabPage_msg.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage_msg.Size = new System.Drawing.Size(904, 536);
            this.tabPage_msg.TabIndex = 2;
            this.tabPage_msg.Text = "Messages";
            this.tabPage_msg.UseVisualStyleBackColor = true;
            // 
            // performanceToolStripMenuItem
            // 
            this.performanceToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.heavyTestToolStripMenuItem});
            this.performanceToolStripMenuItem.Name = "performanceToolStripMenuItem";
            this.performanceToolStripMenuItem.Size = new System.Drawing.Size(94, 21);
            this.performanceToolStripMenuItem.Text = "Performance";
            // 
            // heavyTestToolStripMenuItem
            // 
            this.heavyTestToolStripMenuItem.Name = "heavyTestToolStripMenuItem";
            this.heavyTestToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.heavyTestToolStripMenuItem.Text = "Heavy Test";
            this.heavyTestToolStripMenuItem.Click += new System.EventHandler(this.heavyTestToolStripMenuItem_Click);
            // 
            // CSharpMapContainer
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(912, 562);
            this.Controls.Add(this.tabControl_map);
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "CSharpMapContainer";
            this.Text = "Map container";
            this.tabControl_map.ResumeLayout(false);
            this.tabPage_map1.ResumeLayout(false);
            this.tabPage_map1.PerformLayout();
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.axqtaxviewer_planetosm1)).EndInit();
            this.tabPage_map2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.axqtaxviewer_planetosm2)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TabControl tabControl_map;
        private System.Windows.Forms.TabPage tabPage_map1;
        private System.Windows.Forms.TabPage tabPage_map2;
        private System.Windows.Forms.TabPage tabPage_msg;
        private Axqtaxviewer_planetosmLib.Axqtaxviewer_planetosm axqtaxviewer_planetosm1;
        private Axqtaxviewer_planetosmLib.Axqtaxviewer_planetosm axqtaxviewer_planetosm2;
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem connectionsToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem getConnectionAddressToolStripMenuItem;
        private System.Windows.Forms.Label label_msg;
        private System.Windows.Forms.ListBox listBox_msg;
        private System.Windows.Forms.ToolStripMenuItem performanceToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem heavyTestToolStripMenuItem;
    }
}

