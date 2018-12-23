#include "ToonixGLDrawer.h"

#include <xsi_graphicsequencercontext.h>
#include <xsi_application.h>
#include <xsi_selection.h>

#include <assert.h>

bool TOONIXPASSINITIALIZED = false;
CString TOONIXMATERIALNAME = L"";


void ToonixPass_Init( XSI::CRef in_pSequencerContext, LPVOID *in_pUserData )
{
	//
	// Softimage's plugin registrar calls this to initialize the display pass.
	// Now, we must register the display pass with the Graphic Sequencer
	//
	
	XSI::GraphicSequencerContext l_vGraphicSequencerContext = in_pSequencerContext;

	// make sure the cast succeeded
	assert ( l_vGraphicSequencerContext.IsValid() );

	// get the sequencer from the context object
	XSI::CGraphicSequencer in_pSequencer = l_vGraphicSequencerContext.GetGraphicSequencer ();

	in_pSequencer.RegisterDisplayCallback ( L"ToonixPass", 0, XSI::siPass, XSI::siCustom, L"ToonixPass" );

	
}

void ToonixPass_Execute( XSI::CRef in_pSequencerContext, LPVOID *in_pUserData )
{
	/*
	Application app;
	Model root = app.GetActiveSceneRoot();

	X3DObject myCube;
	root.AddGeometry( L"Cube",L"MeshSurface",L"" ,myCube );

	Material myMaterial;
	myCube.AddMaterial(L"Phong",false,L"",myMaterial);

	CRefArray refShaders = myMaterial.GetShaders();
	for(LONG i = 0;i < refShaders.GetCount(); ++i)
	{
		app.LogMessage( L"The shader: " +
						Shader(refShaders[i]).GetFullName() );
	}

	*/
	
	Application app;
	Model root = app.GetActiveSceneRoot();

	CRefArray materials = root.GetMaterials();
	Application().LogMessage(L"Nb Materials found : "+(CString)materials.GetCount());
	CRefArray shaders;
	for(ULONG m=0;m<materials.GetCount();m++)
	{
		Material material(materials[m]);
		shaders = material.GetAllShaders();
		for(ULONG s=0;s<shaders.GetCount();s++)
		{
			Shader shader(shaders[s]);
			app.LogMessage(shader.GetFullName());
		}
	}

	XSI::GraphicSequencerContext l_vGraphicSequencerContext = in_pSequencerContext;
	assert ( l_vGraphicSequencerContext.IsValid() );
	XSI::CGraphicSequencer in_pSequencer = l_vGraphicSequencerContext.GetGraphicSequencer ();
	in_pSequencer.RenderSceneUsingMode ( siRealtimePortMaterial, siRenderDefault );

	//in_pSequencer.RenderSceneUsingMode(siConstant,siRenderDefault);
	//
	// Grab the current selection list
	//
	
	Selection sel = app.GetSelection();
	CRefArray array(sel.GetArray());

	//
	// Render the scene in hidden line
	//

	in_pSequencer.RenderSceneUsingMode ( siHiddenLineRemoval, siRenderDefault );

	//
	// Now render over the selection list using the material attached to the realtime port
	//

	in_pSequencer.RenderListUsingMode ( array, siRealtimePortMaterial );

	//
	// Note that for the sake of simplicity, we are rendering the object using built in passes 
	// (HiddenLine and RealtimePort).But you have much more flexibility when using RenderSceneUsingMaterial 
	// and provide your own shader ie: in_pSequencer->RenderSceneUsingMaterial ( L"MySuperShader", siRenderDefault );
	//
	//
	// Using Custom Display passes become very interesting when they are use in conjuncture with pixel buffers
	// (ie. accelerated offscreen OpenGL buffers). You can create your pbuffer, make it the current rendering
	// context and use the Sequencer to render the scene. Then, you can composite this pbuffer with other 
	// buffers, use multipass, blur them. etc. etc.
	//
	// Using this technique, you can do stuff like realtime shadows, reflections or other scene-wide effects.
	//
	
	return;
}

void	ToonixPass_Term			( XSI::CRef in_pSequencerContext, LPVOID *in_pUserData )
{

}

void	ToonixPass_InitInstance	( XSI::CRef in_pSequencerContext, LPVOID *in_pUserData )
{
	Application app;
	Model root = app.GetActiveSceneRoot();

	CRefArray materials = root.GetMaterials();
	Application().LogMessage(L"Nb Materials found : "+(CString)materials.GetCount());
	CRefArray shaders;
	for(ULONG m=0;m<materials.GetCount();m++)
	{
		Material material(materials[m]);
		shaders = material.GetAllShaders();
		for(ULONG s=0;s<shaders.GetCount();s++)
		{
			Shader shader(shaders[s]);
			app.LogMessage(shader.GetFullName());
		}
	}
	TOONIXMATERIALNAME = L" Hello filthy motherfucker....";
	TOONIXPASSINITIALIZED = true;
	Application().LogMessage(L"ToonixPass Init Instance Called...");
}

void	ToonixPass_TermInstance	( XSI::CRef in_pSequencerContext, LPVOID *in_pUserData )
{
	Application().LogMessage(L" Toonix Material : "+TOONIXMATERIALNAME);
	Application().LogMessage(L"ToonixPass Term Instance Called...");
	TOONIXMATERIALNAME = L"";
	TOONIXPASSINITIALIZED = false;
}
