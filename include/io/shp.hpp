#ifndef IO_SHP_HPP
#define IO_SHP_HPP

#include "geometry/geometry.hpp"
#include "plu/Lot.hpp"
#include "plu/Road.hpp"
#include "plu/Bldg.hpp"
#include <map>
#include <vector>
#include <boost/filesystem.hpp>

namespace io{

    //typedef geometry::Simple_cartesian<double> K;
    //typedef geometry::Iso_rectangle_2_traits<K>::type Iso_rectangle_2;

    void loadLots(const char* file, std::map<int,Lot>& lots)
    {
        OGRRegisterAll();
        OGRDataSource *poDS = OGRSFDriverRegistrar::Open(file,FALSE);
        OGRLayer *poLayer = poDS->GetLayer(0);
        poLayer->ResetReading();
        OGRFeature *poFeature;

        int index = -1;
        while(poFeature = poLayer->GetNextFeature())
        {
            OGRPolygon* ply = (OGRPolygon*)(poFeature->GetGeometryRef());
            ply->closeRings();
            ++index;
            lots.insert(std::pair<int,Lot>(index,Lot(index,ply)));

            //lots[++index] = Lot(index,(OGRPolygon*)geom);

            OGRFeature::DestroyFeature( poFeature );
        }
        OGRDataSource::DestroyDataSource( poDS );
    }

    void exportLots(const char* file,std::map<int,Lot>& lots)
    {
        OGRRegisterAll();
        OGRSFDriver *poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName("ESRI Shapefile");
        OGRDataSource *poDS;
        OGRLayer *poLayer;

        if(boost::filesystem::exists(file))
            remove(file);

        poDS = poDriver->CreateDataSource( file, NULL );
        poLayer =poDS->CreateLayer( "lot", NULL, wkbPolygon, NULL );


        std::map<int,Lot>::iterator it;
        for(it=lots.begin();it!=lots.end();++it)
        {
            OGRFeature *poFeature = OGRFeature::CreateFeature( poLayer->GetLayerDefn() );
            poFeature->SetGeometry(it->second._polygon);
            poLayer->CreateFeature(poFeature);
            OGRFeature::DestroyFeature(poFeature);

        }
        OGRDataSource::DestroyDataSource( poDS );
    }

    //temporary method
    void exportBorderSegs(const char* file, std::map<int,Lot>& lots)
    {
        OGRRegisterAll();
        OGRSFDriver *poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName("ESRI Shapefile");
        OGRDataSource *poDS;
        OGRLayer *poLayer;

        if(boost::filesystem::exists(file))
            remove(file);

        poDS = poDriver->CreateDataSource( file, NULL );
        poLayer =poDS->CreateLayer( "borderSegments", NULL, wkbLineString, NULL );
        OGRFieldDefn fLotID("lotID",OFTInteger),fSegID("segID",OFTInteger),fBorderType("borderType", OFTString),fBorderID("borderID",OFTInteger);
        poLayer->CreateField( &fLotID );
        poLayer->CreateField( &fSegID );
        poLayer->CreateField( &fBorderType );
        poLayer->CreateField( &fBorderID );

        std::map<int,Lot>::iterator it;
        for(it=lots.begin();it!=lots.end();++it)
        {
            std::map<int,BorderSeg>& segs = it->second._borderSegs;
            std::map<int,BorderSeg>::iterator iter;
            for(iter=segs.begin();iter!=segs.end();++iter)
            {
                OGRFeature *poFeature = OGRFeature::CreateFeature( poLayer->GetLayerDefn() );
                poFeature->SetField("lotID",it->first);
                poFeature->SetField("segID",iter->first);
                poFeature->SetField("borderType","Unknown");
                poFeature->SetField("borderID",-1);
                OGRLineString line;
                line.addPoint(iter->second.getGeom().source().x(),iter->second.getGeom().source().y());
                line.addPoint(iter->second.getGeom().target().x(),iter->second.getGeom().target().y());
                poFeature->SetGeometry(&line);
                poLayer->CreateFeature(poFeature);
                OGRFeature::DestroyFeature(poFeature);
            }
        }
        OGRDataSource::DestroyDataSource( poDS );

    }

    void completeBorders(const char* file, std::map<int,Lot>& lots)
    {
        OGRRegisterAll();
        OGRDataSource *poDS = OGRSFDriverRegistrar::Open(file,FALSE);
        OGRLayer *poLayer = poDS->GetLayer(0);
        poLayer->ResetReading();
        OGRFeature *poFeature;

        int lotID = -1, segID = -1, borderID = -1;
        const char* type;
        while(poFeature = poLayer->GetNextFeature())
        {

            lotID = poFeature->GetFieldAsInteger("lotID");
            segID = poFeature->GetFieldAsInteger("segID");
            type = poFeature->GetFieldAsString("borderType");
            borderID = poFeature->GetFieldAsInteger("borderID");

            BorderType bType;

            if(strcmp(type,"Front")==0)
                bType = FrontBorder;

            else if(strcmp(type,"Side")==0)
                bType = SideBorder;

            else if(strcmp(type,"Back")==0)
                bType = BackBorder;

            lots[lotID]._borderSegs[segID].setType(bType);
            lots[lotID]._borderSegs[segID].setBorderID(borderID);

            //form borders
            if(lots[lotID]._borders.find(borderID)==lots[lotID]._borders.end())
                lots[lotID]._borders[borderID] = Border(lotID,borderID,bType);
            lots[lotID]._borders[borderID].addSeg(&(lots[lotID]._borderSegs[segID]));

            OGRFeature::DestroyFeature(poFeature);
        }
        OGRDataSource::DestroyDataSource(poDS);
    }

    void loadRoads(const char* file, std::map<int,Road>& roads)
    {
        OGRRegisterAll();
        OGRDataSource *poDS = OGRSFDriverRegistrar::Open(file,FALSE);
        OGRLayer *poLayer = poDS->GetLayer(0);
        poLayer->ResetReading();
        OGRFeature *poFeature;

        int index = -1;
        while(poFeature = poLayer->GetNextFeature())
        {
            OGRLineString* road = (OGRLineString*)(poFeature->GetGeometryRef());
            double width = poFeature->GetFieldAsDouble("LARGEUR");
            ++index;
            roads.insert(std::pair<int,Road>(index,Road(index,road,width)));

            //lots[++index] = Lot(index,(OGRPolygon*)geom);

            OGRFeature::DestroyFeature( poFeature );
        }
        OGRDataSource::DestroyDataSource( poDS );
    }

    template<typename Configuration>
    void save_config2shp(const char* file, const Configuration& config,Lot* lot)
    {
        OGRRegisterAll();
        OGRSFDriver *poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName("ESRI Shapefile");
        OGRDataSource *poDS;
        OGRLayer *poLayer;

        if(boost::filesystem::exists(file))
            remove(file);

        poDS = poDriver->CreateDataSource( file, NULL );
        poLayer =poDS->CreateLayer( "building", NULL, wkbPolygon, NULL );
        OGRFieldDefn idField("ID",OFTInteger),lotField("lotID",OFTInteger),hField("height", OFTReal);
        poLayer->CreateField( &idField );
        poLayer->CreateField( &lotField );
        poLayer->CreateField( &hField );

        int id = -1;

        typename Configuration::const_iterator it = config.begin(), end = config.end();
        for (; it != end; ++it)
        {
            OGRLinearRing ring;
            for(int i=0;i<5;i++)
            {
                double x = CGAL::to_double(config[it].rect_2.point(i).x());
                double y = CGAL::to_double(config[it].rect_2.point(i).y());
                ring.addPoint(x,y);
            }
            OGRPolygon ply;
            ply.addRing(&ring);

            OGRFeature *poFeature = OGRFeature::CreateFeature( poLayer->GetLayerDefn() );
            poFeature->SetField("ID",++id);
            poFeature->SetField("lotID",lot->_id);
            poFeature->SetField("height",config[it].h);
            poFeature->SetGeometry(&ply);

            poLayer->CreateFeature(poFeature);
            OGRFeature::DestroyFeature(poFeature);

        }

        OGRDataSource::DestroyDataSource( poDS );
    }




    void save_bldgs2shp(const char* file, std::vector<Bldg>& bldgs)
    {
        OGRRegisterAll();
        OGRSFDriver *poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName("ESRI Shapefile");
        OGRDataSource *poDS;
        OGRLayer *poLayer;

        if(boost::filesystem::exists(file))
            remove(file);

        poDS = poDriver->CreateDataSource( file, NULL );
        poLayer =poDS->CreateLayer( "building", NULL, wkbPolygon, NULL );
        OGRFieldDefn idField("ID",OFTInteger),lotField("lotID",OFTInteger),hField("height", OFTReal);
        poLayer->CreateField( &idField );
        poLayer->CreateField( &lotField );
        poLayer->CreateField( &hField );

        int id = -1;
        for (int i=0;i<bldgs.size();++i)
        {
            OGRPolygon ply;
            SFCGAL2OGR(bldgs[i]._footprint,&ply);

            OGRFeature *poFeature = OGRFeature::CreateFeature( poLayer->GetLayerDefn() );
            poFeature->SetField("ID",++id);
            poFeature->SetField("lotID",bldgs[i]._lot->_id);
            poFeature->SetField("height",bldgs[i]._h);
            poFeature->SetGeometry(&ply);

            poLayer->CreateFeature(poFeature);
            OGRFeature::DestroyFeature(poFeature);

        }

        OGRDataSource::DestroyDataSource( poDS );
    }

    //only for tests
    void loadTestBldgs(const char* folder, int n,std::map<int,std::vector<Bldg> >& test_bldgs)
    {
        OGRRegisterAll();
        test_bldgs.clear();

         std::string file("");
         for(int i=0;i<n;++i)
         {
            file = file + folder + "/bldgs_final_exp"+ boost::lexical_cast<std::string>(i) + ".shp";

            OGRDataSource *poDS = OGRSFDriverRegistrar::Open(file.c_str(),FALSE);
            OGRLayer *poLayer = poDS->GetLayer(0);
            poLayer->ResetReading();
            OGRFeature *poFeature;


            while(poFeature = poLayer->GetNextFeature())
            {
                OGRPolygon* ply = (OGRPolygon*)(poFeature->GetGeometryRef());
                double h = poFeature->GetFieldAsDouble("height");
                test_bldgs[i].push_back(Bldg(*ply,h));

                OGRFeature::DestroyFeature( poFeature );
            }
            OGRDataSource::DestroyDataSource( poDS );
            file = "";
         }



    }

}// namespace io

#endif // IO_SHP_HPP
