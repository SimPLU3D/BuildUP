#ifndef IO_SHP_HPP
#define IO_SHP_HPP

//#include "geometry/geometry.hpp"
#include "Lot.hpp"
#include "Building.hpp"
#include <map>
#include <vector>
#include <boost/filesystem.hpp>

namespace io{

    void loadLots(const char* file, std::map<int,Lot>& lots)
    {
        OGRRegisterAll();
        OGRDataSource *poDS = OGRSFDriverRegistrar::Open(file,FALSE);
        OGRLayer *poLayer = poDS->GetLayer(0);
        poLayer->ResetReading();
        OGRFeature *poFeature;

        int index = -1;
        while( (poFeature = poLayer->GetNextFeature()) )
        {
            OGRPolygon* ply = (OGRPolygon*)(poFeature->GetGeometryRef());
            ply->closeRings();
            ++index;
            lots.insert(std::make_pair(index,Lot(index,ply)));
            OGRFeature::DestroyFeature( poFeature );
        }
        OGRDataSource::DestroyDataSource( poDS );
    }

    void loadBorders(const char* file, std::map<int,Lot>& lots)
    {
        OGRRegisterAll();
        OGRDataSource *poDS = OGRSFDriverRegistrar::Open(file,FALSE);
        OGRLayer *poLayer = poDS->GetLayer(0);
        poLayer->ResetReading();
        OGRFeature *poFeature;

        int lotID = -1, segID = -1, borderID = -1;
        int lotIDCur = -1;
        const char* typeName;
        while( (poFeature = poLayer->GetNextFeature()) )
        { //travers each border segments

            lotID = poFeature->GetFieldAsInteger("lotID");
            segID = poFeature->GetFieldAsInteger("segID");
            typeName = poFeature->GetFieldAsString("borderType");
            borderID = poFeature->GetFieldAsInteger("borderID");

            BorderType bType;

            if(strcmp(typeName,"Front")==0)
                bType = BorderType::Front;

            else if(strcmp(typeName,"Side")==0)
                bType = BorderType::Side;

            else if(strcmp(typeName,"Back")==0)
                bType = BorderType::Back;

            else if(strcmp(typeName,"Corner")==0)
                bType = BorderType::Corner;

            else
                bType = BorderType::Unknown;

            OGRLineString* line = (OGRLineString*)(poFeature->GetGeometryRef());

            //insert border segment
            Lot::Point_2 pt1(line->getX(0),line->getY(0));
            Lot::Point_2 pt2(line->getX(1),line->getY(1));
            Lot::Segment_2 s(pt1,pt2);
            BorderSeg bSeg(lotID,segID,s,bType,borderID);
            lots[lotID].insert_borderSeg(segID, bSeg);

            //insert new border
            if(!lots[lotID].hasBorder(borderID))
            {
                Border border(lotID,borderID,bType,typeName);
                lots[lotID].insert_border(borderID,border);
            }

            //add current segment to existing border
            lots[lotID].add_seg2border(segID,borderID);

            //set name_borders for previous lot (borders are completed)
            if(lotIDCur!=-1 && lotIDCur!=lotID)
                lots[lotIDCur].set_name_borders();
            lotIDCur = lotID;

            OGRFeature::DestroyFeature(poFeature);
        }

        //set name_borders for the last lot
        if(lotIDCur!=-1)
            lots[lotIDCur].set_name_borders();


        OGRDataSource::DestroyDataSource(poDS);
    }

//    void loadRoads(const char* file, std::map<int,Road>& roads)
//    {
//        OGRRegisterAll();
//        OGRDataSource *poDS = OGRSFDriverRegistrar::Open(file,FALSE);
//        OGRLayer *poLayer = poDS->GetLayer(0);
//        poLayer->ResetReading();
//        OGRFeature *poFeature;
//
//        int index = -1;
//        while( (poFeature = poLayer->GetNextFeature()) )
//        {
//            OGRLineString* road = (OGRLineString*)(poFeature->GetGeometryRef());
//            double width = poFeature->GetFieldAsDouble("LARGEUR");
//            ++index;
//            roads.insert(std::pair<int,Road>(index,Road(index,road,width)));
//
//            //lots[++index] = Lot(index,(OGRPolygon*)geom);
//
//            OGRFeature::DestroyFeature( poFeature );
//        }
//        OGRDataSource::DestroyDataSource( poDS );
//    }

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
                double x = geometry::to_double(config[it].rect_2.point(i).x());
                double y = geometry::to_double(config[it].rect_2.point(i).y());
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
        for (size_t i=0;i<bldgs.size();++i)
        {
            OGRPolygon ply;
            SFCGAL2OGR(bldgs[i]._footprint,&ply);

            OGRFeature *poFeature = OGRFeature::CreateFeature( poLayer->GetLayerDefn() );
            poFeature->SetField("ID",++id);
            poFeature->SetField("lotID",bldgs[i]._idLot);
            poFeature->SetField("height",bldgs[i]._h);
            poFeature->SetGeometry(&ply);

            poLayer->CreateFeature(poFeature);
            OGRFeature::DestroyFeature(poFeature);

        }

        OGRDataSource::DestroyDataSource( poDS );
    }


/******************************private interface*******************************************/

    void saveLots(const char* file,std::map<int,Lot>& lots)
    { //save translated lots to shapefile
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
            poFeature->SetGeometry(it->second.polygon());
            poLayer->CreateFeature(poFeature);
            OGRFeature::DestroyFeature(poFeature);

        }
        OGRDataSource::DestroyDataSource( poDS );
    }

    void saveBorderSegs(const char* file, std::map<int,Lot>& lots)
    { //extract and save border segments (without type info) to shapefile so as to add border type info manually
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
            std::map<int,BorderSeg> segs;
            it->second.extractBorderSegs(segs);
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

    void loadEvolution(const char* txt,std::map< int,std::vector<Bldg> >& iter_bldgs)
    {
        std::ifstream in(txt);
        if(in.is_open())
        {
            int iter;
            double x,y,h;
            in>>iter;
            while(!in.eof())
            {
                std::vector<Point> points;
                for(int i=0;i<5;++i)
                {
                    in>>x;
                    in>>y;
                    points.push_back(Point(x,y,0));
                }
                in>>h;
                LineString ring(points);
                Polygon footprint(ring);

                iter_bldgs[iter].push_back(Bldg(footprint,h));

                in>>iter;
            }
        }
    }


//    //only for tests
//    void loadTestBldgs(const char* folder, int n,std::map<int,std::vector<Bldg> >& test_bldgs)
//    {
//        OGRRegisterAll();
//        test_bldgs.clear();
//
//         std::string file("");
//         for(int i=0;i<n;++i)
//         {
//            file = file + folder + "/bldgs_final_exp"+ boost::lexical_cast<std::string>(i) + ".shp";
//
//            OGRDataSource *poDS = OGRSFDriverRegistrar::Open(file.c_str(),FALSE);
//            OGRLayer *poLayer = poDS->GetLayer(0);
//            poLayer->ResetReading();
//            OGRFeature *poFeature;
//
//
//            while( (poFeature = poLayer->GetNextFeature()) )
//            {
//                OGRPolygon* ply = (OGRPolygon*)(poFeature->GetGeometryRef());
//                double h = poFeature->GetFieldAsDouble("height");
//                test_bldgs[i].push_back(Bldg(*ply,h));
//
//                OGRFeature::DestroyFeature( poFeature );
//            }
//            OGRDataSource::DestroyDataSource( poDS );
//            file = "";
//         }
//    }

}// namespace io

#endif // IO_SHP_HPP
