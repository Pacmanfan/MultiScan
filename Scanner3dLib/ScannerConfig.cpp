
#include "ScannerConfig.h"

ScannerConfig::ScannerConfig(void)
{

}

ScannerConfig::~ScannerConfig(void)
{	

}

bool ScannerConfig::Save(FILE *fp)
{
	m_camera.Save(fp);
	fwrite(&m_brightnessthreshold,sizeof(m_brightnessthreshold),1,fp);
	fwrite(&m_usecanny,sizeof(m_usecanny),1,fp);
	fwrite(&m_canny_threshold1,sizeof(m_canny_threshold1),1,fp);
	fwrite(&m_canny_threshold2,sizeof(m_canny_threshold2),1,fp);
	fwrite(&m_canny_apertureSize,sizeof(m_canny_apertureSize),1,fp);

	return true;
}
bool ScannerConfig::Load(FILE *fp)
{
	m_camera.Load(fp);
	fread(&m_brightnessthreshold,sizeof(m_brightnessthreshold),1,fp);
	fread(&m_usecanny,sizeof(m_usecanny),1,fp);
	fread(&m_canny_threshold1,sizeof(m_canny_threshold1),1,fp);
	fread(&m_canny_threshold2,sizeof(m_canny_threshold2),1,fp);
	fread(&m_canny_apertureSize,sizeof(m_canny_apertureSize),1,fp);
	return true;
}
