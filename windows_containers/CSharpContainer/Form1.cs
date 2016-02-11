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
    }
}
