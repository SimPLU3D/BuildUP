#ifndef IO_FILE_HPP
#define IO_FILE_HPP

#include "buildup/plu/Lot.hpp"
#include "buildup/plu/Building.hpp"
#include <map>
#include <vector>
#include <gdal/ogrsf_frmts.h>

namespace io
{
    void load_lots_shp(const char* file, std::map<int,Lot>& lots);
    void load_borders_shp(const char* file, std::map<int,Lot>& lots);
    void load_bldgsFinal_shp(std::string& dir, int n,std::map<int,std::vector<Building> >& exp_bldgs);
    void load_bldgsEvolution_txt(const char* txt,std::map< int,std::vector<Building> >& iter_bldgs);

    template<typename Configuration>
    void save_config2shp(const Configuration& config,const char* file,const int idLot,double transX=0,double transY=0)
    {
        OGRRegisterAll();
        OGRSFDriver *poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName("ESRI Shapefile");
        OGRDataSource *poDS;
        OGRLayer *poLayer;

        poDS = poDriver->CreateDataSource( file, NULL );
        poLayer =poDS->CreateLayer( "building", NULL, wkbPolygon, NULL );
        OGRFieldDefn idField("ID",OFTInteger)
        ,lotField("lotID",OFTInteger)
        ,hField("height", OFTReal)
        ,fX("centerX",OFTReal)
        ,fY("centerY",OFTReal)
        ,fW("width",OFTReal)
        ,fL("length",OFTReal)
        ,fT("theta",OFTReal);
        poLayer->CreateField( &idField );
        poLayer->CreateField( &lotField );
        poLayer->CreateField( &hField );
        poLayer->CreateField( &fX );
        poLayer->CreateField( &fY );
        poLayer->CreateField( &fW );
        poLayer->CreateField( &fL );
        poLayer->CreateField( &fT );


        int id = -1;

        typename Configuration::iterator it = config.begin(), end = config.end();
        for (; it != end; ++it)
        {

            OGRLinearRing ring;
            for(int i=0; i<5; i++)
            {
                double x = config[it].bottom().point(i).x()+transX;
                double y = config[it].bottom().point(i).y()+transY;
                ring.addPoint(x,y);
            }
            OGRPolygon ply;
            ply.addRing(&ring);

            OGRFeature *poFeature = OGRFeature::CreateFeature( poLayer->GetLayerDefn() );
            poFeature->SetField("ID",++id);
            poFeature->SetField("lotID",idLot);
            poFeature->SetField("height",config[it].h());
            poFeature->SetField("centerX",config[it].centerX());
            poFeature->SetField("centerY",config[it].centerY());
            poFeature->SetField("width",config[it].width());
            poFeature->SetField("length",config[it].length());
            poFeature->SetField("theta",config[it].theta());
            poFeature->SetGeometry(&ply);

            poLayer->CreateFeature(poFeature);
            OGRFeature::DestroyFeature(poFeature);

        }

        OGRDataSource::DestroyDataSource( poDS );
    }

}
#endif //IO_FILE_HPP
