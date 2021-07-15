#ifndef SHWFILES_H_INCLUDED
#define SHWFILES_H_INCLUDED

/***************************************************************
 * Name:      SHWFilesApp.h
 * Purpose:   Defines Application Class
 * Author:    BaeByoungYong (bbygoodluck@gmail.com)
 * Created:   2021-06-29
 * Copyright: BaeByoungYong (https://github.com/bbygoodluck)
 * License:
 **************************************************************/

class CSHWFilesApp : public wxApp
{
public:
	explicit CSHWFilesApp();
	virtual ~CSHWFilesApp();

	bool OnInit() override;
	int OnExit() override;

private:
	bool Initialize();
	bool LoadJsonConfigs();

private:
	bool m_bAddFontRC = false;
#ifdef NDEBUG
	wxSingleInstanceChecker* m_pSingleInstance;
#endif
};

#endif // SHWFILES_H_INCLUDED
