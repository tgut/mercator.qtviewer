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
            this.tabPage_map2 = new System.Windows.Forms.TabPage();
            this.tabPage_msg = new System.Windows.Forms.TabPage();
            this.axqtaxviewer_planetosm1 = new Axqtaxviewer_planetosmLib.Axqtaxviewer_planetosm();
            this.axqtaxviewer_planetosm2 = new Axqtaxviewer_planetosmLib.Axqtaxviewer_planetosm();
            this.listBox_msg = new System.Windows.Forms.ListBox();
            this.button1 = new System.Windows.Forms.Button();
            this.tabControl_map.SuspendLayout();
            this.tabPage_map1.SuspendLayout();
            this.tabPage_map2.SuspendLayout();
            this.tabPage_msg.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.axqtaxviewer_planetosm1)).BeginInit();
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
            this.tabControl_map.Size = new System.Drawing.Size(804, 520);
            this.tabControl_map.TabIndex = 0;
            // 
            // tabPage_map1
            // 
            this.tabPage_map1.Controls.Add(this.button1);
            this.tabPage_map1.Controls.Add(this.axqtaxviewer_planetosm1);
            this.tabPage_map1.Location = new System.Drawing.Point(4, 22);
            this.tabPage_map1.Name = "tabPage_map1";
            this.tabPage_map1.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage_map1.Size = new System.Drawing.Size(796, 494);
            this.tabPage_map1.TabIndex = 0;
            this.tabPage_map1.Text = "Map1";
            this.tabPage_map1.UseVisualStyleBackColor = true;
            // 
            // tabPage_map2
            // 
            this.tabPage_map2.Controls.Add(this.axqtaxviewer_planetosm2);
            this.tabPage_map2.Location = new System.Drawing.Point(4, 22);
            this.tabPage_map2.Name = "tabPage_map2";
            this.tabPage_map2.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage_map2.Size = new System.Drawing.Size(796, 494);
            this.tabPage_map2.TabIndex = 1;
            this.tabPage_map2.Text = "Map2";
            this.tabPage_map2.UseVisualStyleBackColor = true;
            // 
            // tabPage_msg
            // 
            this.tabPage_msg.Controls.Add(this.listBox_msg);
            this.tabPage_msg.Location = new System.Drawing.Point(4, 22);
            this.tabPage_msg.Name = "tabPage_msg";
            this.tabPage_msg.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage_msg.Size = new System.Drawing.Size(796, 494);
            this.tabPage_msg.TabIndex = 2;
            this.tabPage_msg.Text = "Messages";
            this.tabPage_msg.UseVisualStyleBackColor = true;
            // 
            // axqtaxviewer_planetosm1
            // 
            this.axqtaxviewer_planetosm1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.axqtaxviewer_planetosm1.Enabled = true;
            this.axqtaxviewer_planetosm1.Location = new System.Drawing.Point(3, 3);
            this.axqtaxviewer_planetosm1.Name = "axqtaxviewer_planetosm1";
            this.axqtaxviewer_planetosm1.OcxState = ((System.Windows.Forms.AxHost.State)(resources.GetObject("axqtaxviewer_planetosm1.OcxState")));
            this.axqtaxviewer_planetosm1.Size = new System.Drawing.Size(790, 488);
            this.axqtaxviewer_planetosm1.TabIndex = 0;
            // 
            // axqtaxviewer_planetosm2
            // 
            this.axqtaxviewer_planetosm2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.axqtaxviewer_planetosm2.Enabled = true;
            this.axqtaxviewer_planetosm2.Location = new System.Drawing.Point(3, 3);
            this.axqtaxviewer_planetosm2.Name = "axqtaxviewer_planetosm2";
            this.axqtaxviewer_planetosm2.OcxState = ((System.Windows.Forms.AxHost.State)(resources.GetObject("axqtaxviewer_planetosm2.OcxState")));
            this.axqtaxviewer_planetosm2.Size = new System.Drawing.Size(790, 488);
            this.axqtaxviewer_planetosm2.TabIndex = 0;
            // 
            // listBox_msg
            // 
            this.listBox_msg.Dock = System.Windows.Forms.DockStyle.Fill;
            this.listBox_msg.FormattingEnabled = true;
            this.listBox_msg.ItemHeight = 12;
            this.listBox_msg.Location = new System.Drawing.Point(3, 3);
            this.listBox_msg.Name = "listBox_msg";
            this.listBox_msg.Size = new System.Drawing.Size(790, 488);
            this.listBox_msg.TabIndex = 0;
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(8, 6);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(75, 23);
            this.button1.TabIndex = 1;
            this.button1.Text = "button1";
            this.button1.UseVisualStyleBackColor = true;
            // 
            // CSharpMapContainer
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(804, 520);
            this.Controls.Add(this.tabControl_map);
            this.Name = "CSharpMapContainer";
            this.Text = "Map container";
            this.tabControl_map.ResumeLayout(false);
            this.tabPage_map1.ResumeLayout(false);
            this.tabPage_map2.ResumeLayout(false);
            this.tabPage_msg.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.axqtaxviewer_planetosm1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.axqtaxviewer_planetosm2)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TabControl tabControl_map;
        private System.Windows.Forms.TabPage tabPage_map1;
        private System.Windows.Forms.TabPage tabPage_map2;
        private System.Windows.Forms.TabPage tabPage_msg;
        private System.Windows.Forms.Button button1;
        private Axqtaxviewer_planetosmLib.Axqtaxviewer_planetosm axqtaxviewer_planetosm1;
        private Axqtaxviewer_planetosmLib.Axqtaxviewer_planetosm axqtaxviewer_planetosm2;
        private System.Windows.Forms.ListBox listBox_msg;
    }
}

