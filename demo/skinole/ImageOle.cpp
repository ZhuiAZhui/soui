#include "StdAfx.h"
#include "ImageOle.h"
#include "../../controls.extend/gif/SSkinGif.h"
#include <tom.h>
#include "../soui/include/sapp.h"


CImageOle::CImageOle(SOUI::SRichEdit *pRichedit)
:m_ulRef(0)
,m_pOleClientSite(NULL)
,m_pAdvSink(NULL)
,m_pSkin(NULL)
,m_iFrame(0)
,m_pRichedit(pRichedit)
,m_nTimePass(0)
{
}

CImageOle::~CImageOle(void)
{
}

HRESULT WINAPI CImageOle::QueryInterface(REFIID iid, void ** ppvObject)
{
	if (iid == IID_IUnknown || iid == IID_IOleObject)
	{
		*ppvObject = (IOleObject *)this;
		((IOleObject *)(*ppvObject))->AddRef();
		return S_OK;
	}
	else if (iid == IID_IViewObject || iid == IID_IViewObject2)
	{
		*ppvObject = (IViewObject2 *)this;
		((IViewObject2 *)(*ppvObject))->AddRef();
		return S_OK;
	}

	return E_NOINTERFACE;
}

ULONG WINAPI CImageOle::AddRef(void)
{
	ULONG lRef = ::InterlockedIncrement(&m_ulRef);
	return lRef;
}

ULONG WINAPI CImageOle::Release(void)
{
    ULONG lRef = ::InterlockedDecrement(&m_ulRef);
	if (lRef == 0)
	{
		delete this;
	}

	return lRef;
}

HRESULT WINAPI CImageOle::SetClientSite(IOleClientSite *pClientSite)
{
    SASSERT(m_pOleClientSite == NULL);
	m_pOleClientSite = pClientSite;
	return S_OK;
}

HRESULT WINAPI CImageOle::GetClientSite(IOleClientSite **ppClientSite)
{
	*ppClientSite = m_pOleClientSite;
	return S_OK;
}

HRESULT WINAPI CImageOle::SetHostNames(LPCOLESTR szContainerApp, LPCOLESTR szContainerObj)
{
	return S_OK;
}

HRESULT WINAPI CImageOle::Close(DWORD dwSaveOption)
{
    m_pOleClientSite = NULL;
    m_pAdvSink = NULL;
    m_pSkin=NULL;
    
	m_pRichedit->GetContainer()->UnregisterTimelineHandler(this);

	return S_OK;
}

HRESULT WINAPI CImageOle::SetMoniker(DWORD dwWhichMoniker, IMoniker *pmk)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOle::GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker **ppmk)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOle::InitFromData(IDataObject *pDataObject, BOOL fCreation, DWORD dwReserved)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOle::GetClipboardData(DWORD dwReserved, IDataObject **ppDataObject)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOle::DoVerb(LONG iVerb, LPMSG lpmsg, IOleClientSite *pActiveSite, LONG lindex, HWND hwndParent, LPCRECT lprcPosRect)
{
	return S_OK;
}

HRESULT WINAPI CImageOle::EnumVerbs(IEnumOLEVERB **ppEnumOleVerb)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOle::Update(void)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOle::IsUpToDate(void)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOle::GetUserClassID(CLSID *pClsid)
{
	*pClsid = IID_NULL;
	return S_OK;
}

HRESULT WINAPI CImageOle::GetUserType(DWORD dwFormOfType, LPOLESTR *pszUserType)
{
	return E_NOTIMPL;
}

// 设置控件可使用的空间（控件在屏幕上使用的区域范围）
HRESULT WINAPI CImageOle::SetExtent(DWORD dwDrawAspect, SIZEL *psizel)
{
	return E_NOTIMPL;
}

// 获取控件可使用的空间
HRESULT WINAPI CImageOle::GetExtent(DWORD dwDrawAspect, SIZEL *psizel)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOle::Advise(IAdviseSink *pAdvSink, DWORD *pdwConnection)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOle::Unadvise(DWORD dwConnection)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOle::EnumAdvise(IEnumSTATDATA **ppenumAdvise)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOle::GetMiscStatus(DWORD dwAspect, DWORD *pdwStatus)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOle::SetColorScheme(LOGPALETTE *pLogpal)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOle::Draw(DWORD dwDrawAspect, LONG lindex, void *pvAspect, 
							   DVTARGETDEVICE *ptd, HDC hdcTargetDev, HDC hdcDraw, 
							   LPCRECTL lprcBounds, LPCRECTL lprcWBounds,
							   BOOL ( WINAPI *pfnContinue )(ULONG_PTR dwContinue), 
							   ULONG_PTR dwContinue)
{
	if (lindex != -1)
		return S_FALSE;

	RECT rcItem;
	rcItem.left   = lprcBounds->left;
	rcItem.top    = lprcBounds->top;
	rcItem.right  = lprcBounds->right;
	rcItem.bottom = lprcBounds->bottom;

	if(m_pSkin)
	{
	    SOUI::IRenderTarget * pRT =NULL;
        RECT rc2={0,0,rcItem.right-rcItem.left,rcItem.bottom-rcItem.top};
	    GETRENDERFACTORY->CreateRenderTarget(&pRT,rc2.right,rc2.bottom);
	    if(pRT)
        {
            HDC hdcSrc=pRT->GetDC();
            ::BitBlt(hdcSrc,0,0,rc2.right,rc2.bottom,hdcDraw,rcItem.left,rcItem.top,SRCCOPY);
            if(m_pSkin->IsClass(SSkinGif::GetClassName()))
            {
                SSkinGif *pSkinGif=static_cast<SSkinGif*>((ISkinObj*)m_pSkin);
                pSkinGif->Draw(pRT,&rc2,m_iFrame);
            }else
            {
                m_pSkin->Draw(pRT,&rc2,0);
            }
            ::BitBlt(hdcDraw,rcItem.left,rcItem.top,rc2.right,rc2.bottom,hdcSrc,0,0,SRCCOPY);
            pRT->ReleaseDC(hdcSrc);
            pRT->Release();
        }
		
	}
	return S_OK;
}

HRESULT WINAPI CImageOle::GetColorSet(DWORD dwDrawAspect, LONG lindex, void *pvAspect, 
									  DVTARGETDEVICE *ptd, HDC hicTargetDev, LOGPALETTE **ppColorSet)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOle::Freeze(DWORD dwDrawAspect, LONG lindex, void *pvAspect, DWORD *pdwFreeze)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOle::Unfreeze(DWORD dwFreeze)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOle::SetAdvise(DWORD aspects, DWORD advf, IAdviseSink *pAdvSink)
{
	if (aspects == DVASPECT_CONTENT && pAdvSink != NULL)
	{
		m_pAdvSink = pAdvSink;
		
        if (m_pSkin != NULL && m_pSkin->IsClass(SSkinGif::GetClassName()))
        {
            SSkinGif *pGif=static_cast<SSkinGif*>((ISkinObj*)m_pSkin);
            m_iFrame=0;
            m_nTimeDelay=pGif->GetFrameDelay(0);
            m_nTimePass=0;
            m_pRichedit->GetContainer()->RegisterTimelineHandler(this);
        }

        return S_OK;
	}else
	{
	    return E_INVALIDARG;
	}
}

HRESULT WINAPI CImageOle::GetAdvise(DWORD *pAspects, DWORD *pAdvf, IAdviseSink **ppAdvSink)
{
	return E_NOTIMPL;
}

HRESULT WINAPI CImageOle::GetExtent(DWORD dwDrawAspect, LONG lindex, DVTARGETDEVICE *ptd, LPSIZEL lpsizel)
{
	if (m_pSkin != NULL)
	{
		SIZE sz=m_pSkin->GetSkinSize();

		HDC hDC = ::GetDC(NULL);
		lpsizel->cx = ::MulDiv(sz.cx, 2540, GetDeviceCaps(hDC, LOGPIXELSX));
		lpsizel->cy = ::MulDiv(sz.cy, 2540, GetDeviceCaps(hDC, LOGPIXELSY));
		::ReleaseDC(NULL, hDC);
	}

	return S_OK;
}


void CImageOle::SetSkinObj( ISkinObj *pSkin )
{
    m_pSkin=pSkin;
}

void CImageOle::OnNextFrame()
{
	m_nTimePass+=10;
	if(m_nTimePass>=m_nTimeDelay)
	{
		SSkinGif *pSkinGif=static_cast<SSkinGif*>((ISkinObj*)m_pSkin);
		SASSERT(pSkinGif);
		m_iFrame++;
		if(m_iFrame==pSkinGif->GetStates())
			m_iFrame=0;

		m_nTimeDelay=pSkinGif->GetFrameDelay(m_iFrame)*10;
		m_nTimePass=0;
		
		if(m_pAdvSink) m_pAdvSink->OnViewChange(DVASPECT_CONTENT,-1);
	}
}

CImageOle * CImageOle::CreateObject( SOUI::SRichEdit *pRichedit )
{
    return new CImageOle(pRichedit);
}

BOOL RichEdit_InsertSkin(SOUI::SRichEdit *pRicheditCtrl, SOUI::ISkinObj *pSkin)
{
	IRichEditOle *pRichEditOle=NULL;
	LRESULT lRes=pRicheditCtrl->SSendMessage(EM_GETOLEINTERFACE,0,(LPARAM)&pRichEditOle);
	if(!pRichEditOle) return FALSE;

	IOleClientSite *pOleClientSite = NULL;
	pRichEditOle->GetClientSite(&pOleClientSite);
	if (NULL == pOleClientSite)
		return FALSE;

	CImageOle *pImageOle = CImageOle::CreateObject(pRicheditCtrl);
	if (NULL == pImageOle)
		return FALSE;

	pImageOle->SetSkinObj(pSkin);

	IOleObject *pOleObject = NULL;
	pImageOle->QueryInterface(IID_IOleObject, (void **)&pOleObject);
	if (NULL == pOleObject)
	{
		pImageOle->Release();
		return FALSE;
	}

	pImageOle->SetClientSite(pOleClientSite);

	HRESULT hr = ::OleSetContainedObject(pOleObject, TRUE);

	REOBJECT reobject = {0};
	reobject.cbStruct = sizeof(REOBJECT);
	reobject.clsid    = CLSID_NULL;
	reobject.cp       = REO_CP_SELECTION;
	reobject.dvaspect = DVASPECT_CONTENT;
    reobject.dwFlags  = REO_BELOWBASELINE;
	reobject.poleobj  = pOleObject;
	reobject.polesite = pOleClientSite;
	reobject.dwUser   = 0;

	pRichEditOle->InsertObject(&reobject);

	pOleObject->Release();
	pOleClientSite->Release();
	pRichEditOle->Release();

	return TRUE;
}

BOOL RichEdit_InsertImage(SOUI::SRichEdit *pRicheditCtrl, LPCTSTR lpszFileName)
{
    SStringW key=S_CT2W(lpszFileName);
    SOUI::SSkinPool *pBuiltinSkinPool = SOUI::SSkinPoolMgr::getSingletonPtr()->GetBuiltinSkinPool();
    SOUI::ISkinObj *pSkin=pBuiltinSkinPool->GetSkin(key);
    if(!pSkin)
    {
        SOUI::SSkinGif *pGifSkin = (SOUI::SSkinGif*)SOUI::SApplication::getSingleton().CreateSkinByName(SSkinGif::GetClassName());
        if(!pGifSkin) return FALSE;
        if(0==pGifSkin->LoadFromFile(lpszFileName))
        {
            pGifSkin->Release();
            return FALSE;
        }

        pBuiltinSkinPool->AddKeyObject(key,pGifSkin);//将创建的skin交给skinpool管理
        pSkin = pGifSkin;
    }
    return RichEdit_InsertSkin(pRicheditCtrl,pSkin);
}