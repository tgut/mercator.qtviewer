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
        public string m_gridLayerName;
        public string m_geomarkerLayerName;
        public void init_layer_names()
        {
            int layers = axqtaxviewer_planetosm1.osm_layer_get_count();
            for (int i = 0; i < layers; ++i)
            {
                string layername = axqtaxviewer_planetosm1.osm_layer_get_name(i);
                if (layername.Contains("grid"))
                    m_gridLayerName = layername;
                if (layername.Contains("geomarker"))
                    m_geomarkerLayerName = layername;
            }
        }
        public CSharpMapContainer()
        {
            InitializeComponent();
            init_layer_names();
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
        //display message for map1
        private void axqtaxviewer_planetosm1_evt_Message(object sender, Axqtaxviewer_planetosmLib.Iqtaxviewer_planetosmEvents_evt_MessageEvent e)
        {
            if (e.p0.IndexOf("MOUSE_MOVE") > 0)
                label_msg.Text = e.p0;
            else
            {
                messageOutput("EVT from " + sender.GetHashCode() + ":\n" + e.p0, 0);
                messageOutput(string2dict(e.p0), 4);
            }
        }
        //display message for map2
        private void axqtaxviewer_planetosm2_evt_Message(object sender, Axqtaxviewer_planetosmLib.Iqtaxviewer_planetosmEvents_evt_MessageEvent e)
        {
            if (e.p0.IndexOf("MOUSE_MOVE") > 0)
                label_msg.Text = e.p0;
            else
            {
                messageOutput("EVT from " + sender.GetHashCode() + ":\n" + e.p0, 0);
                messageOutput(string2dict(e.p0), 4);
            }
        }
        //Add a lot of items to map
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
                axqtaxviewer_planetosm1.osm_layer_call_function(m_geomarkerLayerName,cmd);
            }
        }
        //test function osm_set_remote_address
        //the map will download tiles as needed from this address
        private void setConnectionAddressToolStripMenuItem_Click(object sender, EventArgs e)
        {
            messageOutput("osm_set_remote_address");
            axqtaxviewer_planetosm1.osm_set_remote_address("OSM", "http://tile-a.openstreetmap.fr/hot/%1/%2/%3.png");
            string str = axqtaxviewer_planetosm1.osm_get_remote_address("OSM");
            messageOutput(str, 4);
        }
        //test function osm_get_local_cache
        //the map will download tiles as needed to this address
        private void getCachePathToolStripMenuItem_Click(object sender, EventArgs e)
        {
            messageOutput("osm_get_local_cache");
            string str = axqtaxviewer_planetosm1.osm_get_local_cache("OSM");
            messageOutput(str, 4);

        }
        //test function osm_get_local_cache
        //the map will download tiles as needed to this address
        private void setCachePathToolStripMenuItem_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog fbd = new FolderBrowserDialog();
            fbd.SelectedPath = ".\\";
            if (fbd.ShowDialog() == DialogResult.OK)
            {
                
                messageOutput("osm_set_remote_address");
                axqtaxviewer_planetosm1.osm_set_remote_address("OSM", fbd.SelectedPath);
                string str = axqtaxviewer_planetosm1.osm_get_local_cache("OSM");
                messageOutput(str, 4);
            }

        }
        //cache expire days is a integer beyond which the tile will be downloaded again from remote.
        //this function will get the expiredays from server
        private void getExpireDaysToolStripMenuItem_Click(object sender, EventArgs e)
        {
            messageOutput("osm_get_cache_expire_days");
            int it = axqtaxviewer_planetosm1.osm_get_cache_expire_days("OSM");
            messageOutput(it.ToString(), 4);
        }

        private void setExpireDaysToolStripMenuItem_Click(object sender, EventArgs e)
        {
            messageOutput("osm_set_cache_expire_days");
            int it = axqtaxviewer_planetosm1.osm_get_cache_expire_days("OSM");

            axqtaxviewer_planetosm1.osm_set_cache_expire_days("OSM", it + 1);

            it = axqtaxviewer_planetosm1.osm_get_cache_expire_days("OSM");

            messageOutput(it.ToString(), 4);
        }
        //auto download is the flag that controls the behavior this ocx ctrl have.
        //when it is -1, tiles will be downloaded as needed.
        private void getAutoDownloadToolStripMenuItem_Click(object sender, EventArgs e)
        {
            int t = axqtaxviewer_planetosm1.osm_get_auto_download("OSM");
            messageOutput("osm_get_auto_download");
            messageOutput(t.ToString(), 4);

        }
        //auto download is the flag that controls the behavior this ocx ctrl have.
        //when it is -1, tiles will be downloaded as needed.
        private void setAutoDownloadToolStripMenuItem_Click(object sender, EventArgs e)
        {
            int t = axqtaxviewer_planetosm1.osm_get_auto_download("OSM");
            axqtaxviewer_planetosm1.osm_set_auto_download("OSM", ~t);
            t = axqtaxviewer_planetosm1.osm_get_auto_download("OSM");
            messageOutput("osm_get_auto_download");
            messageOutput(t.ToString(), 4);
        }

        private void getLevelToolStripMenuItem_Click(object sender, EventArgs e)
        {
            int t = axqtaxviewer_planetosm1.osm_get_level();
            messageOutput("osm_get_level");
            messageOutput(t.ToString(), 4);
        }

        private void setLevelToolStripMenuItem_Click(object sender, EventArgs e)
        {
            int t = axqtaxviewer_planetosm1.osm_get_level();
            axqtaxviewer_planetosm1.osm_set_level((t+1)%19);
            t = axqtaxviewer_planetosm1.osm_get_level();
            messageOutput("osm_set_level");
            messageOutput(t.ToString(), 4);
        }

        private void getCenterPosToolStripMenuItem_Click(object sender, EventArgs e)
        {
            double lat = axqtaxviewer_planetosm1.osm_get_center_lat();
            double lon = axqtaxviewer_planetosm1.osm_get_center_lon();
            messageOutput("osm_get_center_lat");
            messageOutput(lat.ToString(), 4);
            messageOutput("osm_get_center_lon");
            messageOutput(lon.ToString(), 4);
        }

        private void setCenterPosToolStripMenuItem_Click(object sender, EventArgs e)
        {
            double lat = axqtaxviewer_planetosm1.osm_get_center_lat();
            double lon = axqtaxviewer_planetosm1.osm_get_center_lon();

            lat = (lat + 181 + 90) % 180 - 90;
            lon = (lon + 361 + 180) % 360 - 180;

            axqtaxviewer_planetosm1.osm_set_center_pos(lat, lon);

            messageOutput("osm_get_center_lat");
            messageOutput(lat.ToString(), 4);
            messageOutput("osm_get_center_lon");
            messageOutput(lon.ToString(), 4);
        }

        private void saveImageToolStripMenuItem_Click(object sender, EventArgs e)
        {
            SaveFileDialog dlg = new SaveFileDialog();
            dlg.Filter = "BMP(*.bmp)|*.bmp|jpg(*.jpg)|*.jpg|png(*.png)|*.png";

            if (dlg.ShowDialog() == DialogResult.OK)
            {
                int res = axqtaxviewer_planetosm1.osm_save_view(dlg.FileName);
                messageOutput("osm_save_view");
                messageOutput(res.ToString(), 4);
            }
        }
        //enum all layers
        private void enumLayersToolStripMenuItem_Click(object sender, EventArgs e)
        {
            messageOutput("enum layers");
            int layers = axqtaxviewer_planetosm1.osm_layer_get_count();
            messageOutput("osm_layer_get_count :"+ layers,4);
            for (int i=0;i<layers;++i)
            {
                string layername = axqtaxviewer_planetosm1.osm_layer_get_name(i);
                messageOutput("osm_layer_get_name :" + layername, 4);

            }
        }
        //set layer visibility
        private void layerVisibilityToolStripMenuItem_Click(object sender, EventArgs e)
        {
            int layer_t = axqtaxviewer_planetosm1.osm_layer_get_visiable("OSM");
            messageOutput("osm_layer_set_visiable");
            messageOutput("old status = "+layer_t,4);
            axqtaxviewer_planetosm1.osm_layer_set_visiable("OSM",~layer_t);
            layer_t = axqtaxviewer_planetosm1.osm_layer_get_visiable("OSM");
            messageOutput("new status = " + layer_t, 4);

        }
        //set layer activity, layer will not recieve mouse messages when it is not active
        private void layerActivityToolStripMenuItem_Click(object sender, EventArgs e)
        {
            int layer_t = axqtaxviewer_planetosm1.osm_layer_get_active("OSM");
            messageOutput("osm_layer_set_active");
            messageOutput("old status = " + layer_t, 4);
            axqtaxviewer_planetosm1.osm_layer_set_active("OSM", ~layer_t);
            layer_t = axqtaxviewer_planetosm1.osm_layer_get_active("OSM");
            messageOutput("new status = " + layer_t, 4);
        }

        private void moveLayerUpToolStripMenuItem_Click(object sender, EventArgs e)
        {
            int n = axqtaxviewer_planetosm1.osm_layer_move_up(m_gridLayerName);
            messageOutput("osm_layer_move_up");
            messageOutput("res = " + n, 4);
        }

        private void moveLayerDownToolStripMenuItem_Click(object sender, EventArgs e)
        {
            int n = axqtaxviewer_planetosm1.osm_layer_move_down(m_gridLayerName);
            messageOutput("osm_layer_move_down");
            messageOutput("res = " + n, 4);
        }

        private void moveLayerTopToolStripMenuItem_Click(object sender, EventArgs e)
        {
            int n = axqtaxviewer_planetosm1.osm_layer_move_top(m_gridLayerName);
            messageOutput("osm_layer_move_top");
            messageOutput("res = " + n, 4);
        }

        private void moveLayerBottomToolStripMenuItem_Click(object sender, EventArgs e)
        {
            int n = axqtaxviewer_planetosm1.osm_layer_move_bottom(m_gridLayerName);
            messageOutput("osm_layer_move_bottom");
            messageOutput("res = " + n, 4);
        }

        private void tabPage_map1_Click(object sender, EventArgs e)
        {

        }
        //the measure function is designed to measure area and length of a polygon on earth.
        
        private void measureOnoffToolStripMenuItem_Click(object sender, EventArgs e)
        {
            messageOutput("grid1::measure control:");
            string res = axqtaxviewer_planetosm1.osm_layer_call_function(
                m_gridLayerName,
                "function=get_ruler_status;");
            Dictionary<string, string> dr = string2dict(res);
            string v;
            if (dr.TryGetValue("status",out v))
            {
                int s = System.Convert.ToInt32(v);
                messageOutput("grid1::get_ruler_status="+ s,4);
                res = axqtaxviewer_planetosm1.osm_layer_call_function(
                m_gridLayerName,
                "function=set_ruler_status;status="+(~s).ToString());

                res = axqtaxviewer_planetosm1.osm_layer_call_function(
                m_gridLayerName,
                "function=get_ruler_status;");

                messageOutput("grid1::set_ruler_status returns " + res, 4);
            }
            
           
        }

        private void getRegionToolStripMenuItem_Click(object sender, EventArgs e)
        {
            messageOutput("grid1::get_polygon:");
            string res = axqtaxviewer_planetosm1.osm_layer_call_function
                (m_gridLayerName, "function=get_polygon;");
            Dictionary<string, string> dr = string2dict(res);
            messageOutput(dr, 4);
        }

        private void updatePointToolStripMenuItem_Click(object sender, EventArgs e)
        {
            messageOutput("geomarker1::update_point");
            //add a round point
            string res = axqtaxviewer_planetosm1.osm_layer_call_function(m_geomarkerLayerName,
                "function=update_point;name=point_001;type=1;" +
                "lat=35.2;lon=101.783;" +
                "style_pen=2;color_pen=0,0,255,128;width_pen=3;" +
                "style_brush=1;color_brush=0,255,0,128;" +
                "color_label=0,0,255,96;weight_label=99;size_label=12;" +
                "width=16;height=20;");
            if (res.Length > 0)
                messageOutput(res);
            //set props
            messageOutput("geomarker1::update_props");
            res = axqtaxviewer_planetosm1.osm_layer_call_function(m_geomarkerLayerName,
                "function=update_props;name=point_001;" +
                "LABEL=Neimeng;EXPRESS=YunDa;Pero=NORMAL;" +
                "CheckTime=2014-12-30 07:18:32;" +
                "From=Huhehaote;To=YinChuan");
            if (res.Length > 0)
                messageOutput(res);
            //add a rect point
            res = axqtaxviewer_planetosm1.osm_layer_call_function(m_geomarkerLayerName,
                "function=update_point;name=point_002;type=2;" +
                "lat=45.2;lon=54.783;" +
                "style_pen=4;color_pen=0,134,255,223;width_pen=2;" +
                "style_brush=2;color_brush=255,55,0,128;" +
                "color_label=111,23,56,96;weight_label=99;size_label=9;" +
                "width=16;height=16;");
            if (res.Length > 0)
                messageOutput(res);
            //set props
            messageOutput("geomarker1::update_props");
            res = axqtaxviewer_planetosm1.osm_layer_call_function(m_geomarkerLayerName,
                "function=update_props;name=point_002;" +
                "LABEL=EMS Global;EXPRESS=EMS;IMM=NORMAL;" +
                "CheckTime=2014-12-31 17:18:32;" +
                "From=HuTaAnna;To=San Oaris");
            if (res.Length > 0)
                messageOutput(res);
        }

        private void updateLineToolStripMenuItem_Click(object sender, EventArgs e)
        {
            messageOutput("geomarker1::update_line");
            string res = axqtaxviewer_planetosm1.osm_layer_call_function(m_geomarkerLayerName,
                "function=update_line;name=line_001;type=3;" +
                "lat0=40;lon0=98;lat1=12;lon1=144;" +
                "style_pen=4;color_pen=255,0,0,96;width_pen=2;");
            if (res.Length > 0)
                messageOutput(res);
            //set props
            messageOutput("geomarker1::update_props");
            res = axqtaxviewer_planetosm1.osm_layer_call_function(m_geomarkerLayerName,
                "function=update_props;name=line_001;" +
                "LABEL=A line;");
            if (res.Length > 0)
                messageOutput(res);
        }

        private void updatePolygonToolStripMenuItem_Click(object sender, EventArgs e)
        {
            messageOutput("geomarker1::update_polygon");
            string res = axqtaxviewer_planetosm1.osm_layer_call_function(m_geomarkerLayerName,
                "function=update_polygon;name=polygon_1;type=4;" +
                "lat0=12.2;lon0=67.3;" +
                "lat1=24.3;lon1=72.8;" +
                "lat2=36.7;lon2=96.5;" +
                "lat3=28.5;lon3=82.2;" +
                "lat4=10.8;lon4=79.4;" +
                "style_pen=2;color_pen=0,0,255,128;" +
                "width_pen=3;style_brush=1;color_brush=0,255,0,128;");
            if (res.Length > 0)
                messageOutput(res);
            //set props
            messageOutput("geomarker1::update_props");
            res = axqtaxviewer_planetosm1.osm_layer_call_function(m_geomarkerLayerName,
                "function=update_props;name=polygon_1;" +
                "LABEL=A polygon;");
            if (res.Length > 0)
                messageOutput(res);

            res = axqtaxviewer_planetosm1.osm_layer_call_function(m_geomarkerLayerName,
                "function=update_polygon;name=polygon_2;type=6;" +
                "lat0=-12.2;lon0=-67.3;" +
                "lat1=-24.3;lon1=-72.8;" +
                "lat2=-36.7;lon2=-86.5;" +
                "lat3=-48.5;lon3=-92.2;" +
                "lat4=-30.8;lon4=-109.4;" +
                "style_pen=3;color_pen=33,255,0,128;" +
                "width_pen=5;style_brush=2;color_brush=0,255,0,128;");
            if (res.Length > 0)
                messageOutput(res);
            //set props
            messageOutput("geomarker1::update_props");
            res = axqtaxviewer_planetosm1.osm_layer_call_function(m_geomarkerLayerName,
                "function=update_props;name=polygon_2;" +
                "LABEL=A multiline;");
            if (res.Length > 0)
                messageOutput(res);
        }

        private void updateIconToolStripMenuItem_Click(object sender, EventArgs e)
        {
            messageOutput("geomarker1::update_icon");
            string res = axqtaxviewer_planetosm1.osm_layer_call_function(m_geomarkerLayerName,
                "function=update_icon;name=icon1;" +
                "lat=12.347364;lon=107.3736438;" +
                "icon=default;scale=1.2;rotate=12;smooth=1;"
                + "color_label=0,0,255,96;weight_label=99;size_label=9;");
            if (res.Length > 0)
                messageOutput(res);
            //set props
            messageOutput("geomarker1::update_props");
            res = axqtaxviewer_planetosm1.osm_layer_call_function(m_geomarkerLayerName,
                "function=update_props;name=icon1;" +
                "LABEL=A icon;");
            if (res.Length > 0)
                messageOutput(res);
        }

        private void existsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            messageOutput("geomarker1::exists");
            string res = axqtaxviewer_planetosm1.osm_layer_call_function(m_geomarkerLayerName,
                "function=exists;name=point_001;");
            if (res.Length > 0)
                messageOutput(res);
        }

        private void deletemarksToolStripMenuItem_Click(object sender, EventArgs e)
        {
            messageOutput("geomarker1::delete_marks");
            string res = axqtaxviewer_planetosm1.osm_layer_call_function(m_geomarkerLayerName,
                "function=delete_marks;name0=point_001;name1=point_002;name2=line_001;name3=polygon_1;"+
                ";name4=polygon_2;name5=icon1");
            if (res.Length > 0)
                messageOutput(string2dict( res));
        }

        private void deletePropsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            messageOutput("geomarker1::delete_props");
            string res = axqtaxviewer_planetosm1.osm_layer_call_function(m_geomarkerLayerName,
                "function=delete_props;name=point_001;prop0=LABEL;prop1=CheckTime");
            if (res.Length > 0)
                messageOutput(string2dict(res));
        }

        private void enumAllInfosToolStripMenuItem_Click(object sender, EventArgs e)
        {
            messageOutput("geomarker1::enum all infos");
            //first, return all names
            string res = axqtaxviewer_planetosm1.osm_layer_call_function(m_geomarkerLayerName,
                "function=mark_names;");
            Dictionary<string, string> dct_names = string2dict(res);
            //for each name, get detailed infomations
            foreach(string name in dct_names.Values)
            {
                messageOutput("info of "+name +":",2);

                res = axqtaxviewer_planetosm1.osm_layer_call_function
                    (m_geomarkerLayerName, "function=mark;name=" + name + ";");
                messageOutput(string2dict(res), 4);
                messageOutput("prop of " + name + ":", 2);

                res = axqtaxviewer_planetosm1.osm_layer_call_function
                    (m_geomarkerLayerName, "function=props;name=" + name + ";");
                messageOutput(string2dict(res), 4);

            }
        }

        private void SaveToDiskToolStripMenuItem_Click(object sender, EventArgs e)
        {
            SaveFileDialog dlg = new SaveFileDialog();
            dlg.Filter = "XML(*.xml)|*.xml";

            if (dlg.ShowDialog() == DialogResult.OK)
            {
                string res = axqtaxviewer_planetosm1.osm_layer_call_function
                    (m_geomarkerLayerName, "function=save_xml;xml=" + dlg.FileName + ";");
                messageOutput("save_xml");
                messageOutput(res, 4);
            }
        }

        private void loadFromDiskToolStripMenuItem_Click(object sender, EventArgs e)
        {
            OpenFileDialog dlg = new OpenFileDialog();
            dlg.Filter = "XML(*.xml)|*.xml";

            if (dlg.ShowDialog() == DialogResult.OK)
            {
                string res = axqtaxviewer_planetosm1.osm_layer_call_function
                    (m_geomarkerLayerName, "function=load_xml;xml=" + dlg.FileName + ";");
                messageOutput("load_xml");
                messageOutput(res, 4);
            }
        }
    }
}
