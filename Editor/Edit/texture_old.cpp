/*
ETexture::ETexture( char *_ShortName ){
	VERIFY( _ShortName );
	strcpy( m_ShortName, _ShortName );
	if( 0==(m_Ref=TM->SearchCore( m_ShortName )) ){
		m_Ref=new ETextureCore( m_ShortName );
		TM->m_Cores.push_back( m_Ref );
    }
	m_Ref->m_RefCount++;
	TM->m_Tex.push_back( this );
}

ETexture::~ETexture(){
	VERIFY( m_Ref );
	TM->m_Tex.remove( this );
	m_Ref->m_RefCount--;
	if( m_Ref->m_RefCount == 0 ){
		TM->m_Cores.remove( m_Ref );
		delete m_Ref; }
}

bool ETexture::MakeTGA( char *filename ){
	VERIFY( filename );
	if( !m_Ref )
		return false;
	return m_Ref->MakeTGA( filename );
}

void ETexture::Bind(){
	VERIFY( m_Ref );
	if( m_Ref != TM->m_Current ){
		TM->m_Current = m_Ref;
		m_Ref->Bind();
    }
}
//----------------------------------------------------
//----------------------------------------------------

ETM::ETM(){
	VERIFY( m_Tex.size() == 0 );
	VERIFY( m_Cores.size() == 0 );
	m_Current = 0;
}

ETM::~ETM(){
	VERIFY( m_Tex.size() == 0 );
	VERIFY( m_Cores.size() == 0 );
}

void ETM::Clear(){
}

void ETM::BindNone(){
	UI->Device.d3d()->SetTexture( 0, 0 );
	m_Current = 0;
}

ETextureCore *ETM::SearchCore( char *_ShortName ){
	VERIFY( _ShortName );
	list<ETextureCore*>::iterator _It = m_Cores.begin();
	for(;_It!=m_Cores.end();_It++)
		if( 0==stricmp((*_It)->m_ShortName,_ShortName) )
			return (*_It);
	return 0;
}

void ETM::ReloadTextures(){
	list<ETextureCore*>::iterator _It = m_Cores.begin();
	for(;_It!=m_Cores.end();_It++){
        SetStatus((*_It)->m_ShortName);
    	(*_It)->DDClear();
	    (*_It)->Unload();
		(*_It)->GetParams();
    }
    SetStatus("");
}
//----------------------------------------------------

void ETM::LoadData(){
	list<ETextureCore*>::iterator _It = m_Cores.begin();
	for(;_It!=m_Cores.end();_It++){
        SetStatus((*_It)->m_ShortName);
        (*_It)->Load();
    }
    SetStatus("");
}

void ETM::UnloadData(){
	list<ETextureCore*>::iterator _It = m_Cores.begin();
	for(;_It!=m_Cores.end();_It++){
        SetStatus((*_It)->m_ShortName);
        (*_It)->Unload();
    }
    SetStatus("");
}
*/