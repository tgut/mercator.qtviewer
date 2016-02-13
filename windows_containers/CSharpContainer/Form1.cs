using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace CSharpContainer
{
    public partial class CSharpMapContainer : Form
    {
        public CSharpMapContainer()
        {
            InitializeComponent();
        }
        //a common message output method
        private void messageOutput(string strmsg, int intend = 0)
        {
            string msg_string = new string(' ', intend);
            msg_string += strmsg;
            listBox_msg.Items.Add(msg_string);
            while (listBox_msg.Items.Count > 1024)
                listBox_msg.Items.RemoveAt(0);
            listBox_msg.SetSelected(listBox_msg.Items.Count - 1, true);
        }
        //a common message output method for paras
        private void messageOutput(Dictionary<string, string> d, int intend = 0)
        {
            string msg_span = new string(' ', intend);
            foreach (string k in d.Keys)
            {
                string v ;
                if (true == d.TryGetValue(k,out v) )
                {
                    string msg = msg_span + k + "=" + v;
                    listBox_msg.Items.Add(msg);
                }
            }
            
            while (listBox_msg.Items.Count > 1024)
                listBox_msg.Items.RemoveAt(0);
            listBox_msg.SetSelected(listBox_msg.Items.Count - 1, true);
        }
        //a common method to convert strings to dictionary.
        private Dictionary<string,string> string2dict(string str)
        {
            char[] spc = { ';' }; 
            Dictionary<string, string> retd = new Dictionary<string, string>();
            string [] prop_pairs = str.Split(spc);
            foreach (string pa in prop_pairs)
            {
                char[] spsub = { '=' };
                string[] prop_keyv = pa.Split(spsub);
                if (prop_keyv.Length >=2)
                {
                    if (retd.ContainsKey(prop_keyv[0]) == false)
                        retd.Add(prop_keyv[0], prop_keyv[1]);
                }
            }
            return retd;
        }
        //Test get tile layer's remote address. In this example, the layername is "OSM"
        private void getConnectionAddressToolStripMenuItem_Click(object sender, EventArgs e)
        {
            messageOutput("osm_get_remote_address:");
            string str = axqtaxviewer_planetosm1.osm_get_remote_address("OSM");
            messageOutput(str,4);
        }

        private void axqtaxviewer_planetosm1_evt_Message(object sender, Axqtaxviewer_planetosmLib.Iqtaxviewer_planetosmEvents_evt_MessageEvent e)
        {
            if (e.p0.IndexOf("MOUSE_MOVE") > 0)
                label_msg.Text = e.p0;
            else
            {
                messageOutput("EVT from " + sender.ToString() + ":" + e.p0, 0);
                messageOutput(string2dict(e.p0), 4);
            }
        }

        private void axqtaxviewer_planetosm2_evt_Message(object sender, Axqtaxviewer_planetosmLib.Iqtaxviewer_planetosmEvents_evt_MessageEvent e)
        {
            if (e.p0.IndexOf("MOUSE_MOVE") > 0)
                label_msg.Text = e.p0;
            else
            {
                messageOutput("EVT from " + sender.ToString() + ":" + e.p0, 0);
                messageOutput(string2dict(e.p0), 4);
            }
        }

        private void heavyTestToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Random rs = new Random();
            //add a lot of marks, testing navigate performance
            for (int i=0;i<16384;++i)
            {
                double lat = rs.NextDouble() * 170 - 85;
                double lon = rs.NextDouble() * 360 - 180;
                int tp = rs.Next(2) + 1;
                int pc_r = rs.Next(256 - 32) + 32;
                int pc_g = rs.Next(256 - 32) + 32;
                int pc_b = rs.Next(256 - 32) + 32;
                int pc_a = rs.Next(256 - 64) + 64;
                int pc_w = rs.Next(4) + 1;
                int pc_style = rs.Next(6) + 1;
                int pb_r = rs.Next(256 - 32) + 32;
                int pb_g = rs.Next(256 - 32) + 32;
                int pb_b = rs.Next(256 - 32) + 32;
                int pb_a = rs.Next(256 - 64) + 64;
                int pb_style = rs.Next(12) + 1;
                int width = rs.Next(24) + 8;
                int height = rs.Next(24) + 8;

                string cmd = 
                    "function=update_point;name=ID"+i.ToString()+";type="+tp.ToString()+
                    ";lat="+ lat.ToString()+ ";lon=" +lon.ToString() +
                    ";style_pen="+ pc_style.ToString()+";color_pen=" + pc_r + "," + pc_g + "," + pc_b + "," + pc_a + ";"+
                    "width_pen=" + pc_w + ";style_brush="+pb_style + ";color_brush=" +
                    pb_r + "," + pb_g + "," + pb_b +"," + pb_a +";"+
                    "color_label=0,0,255,96;weight_label=99;size_label=9;"+
                    "width="+width +";height="+height+";";
                axqtaxviewer_planetosm1.osm_layer_call_function("geomarker1",cmd);
            }
        }
    }
}
