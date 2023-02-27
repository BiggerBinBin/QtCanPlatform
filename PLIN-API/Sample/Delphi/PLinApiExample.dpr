program PLinApiExample;

uses
  Forms,
  Unit1 in 'Unit1.pas' {Form1},
  PLinApi in 'PLinApi.pas',
  GlobalFrameTable in 'GlobalFrameTable.pas';

{$R *.res}

begin

  //ReportMemoryLeaksOnShutdown := DebugHook <> 0;
    
  Application.Initialize;
  Application.MainFormOnTaskbar := True;
  Application.CreateForm(TForm1, Form1);
  Application.Run;
end.
