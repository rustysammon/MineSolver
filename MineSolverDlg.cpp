// MineSolverDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MineSolver.h"
#include "MineSolverDlg.h"
#include ".\minesolverdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//Global variables
HWND g_minesweeperWindow = NULL;   //the minesweeper window that we're playing


// CMineSolverDlg dialog
CMineSolverDlg::CMineSolverDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMineSolverDlg::IDD, pParent)
    , m_minefield(MINEFIELD_WIDTH, MINEFIELD_HEIGHT, MINE_UNKNOWN)
{
	m_gridIndex.x = 0;
	m_gridIndex.y = 0;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMineSolverDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_FEEDBACK, m_ctrlFeedback);
	DDX_Control(pDX, IDC_EDIT_GRIDX, m_ctrlGridX);
	DDX_Control(pDX, IDC_EDIT_GRIDY, m_ctrlGridY);
	DDX_Text(pDX, IDC_EDIT_GRIDX, m_gridIndex.x);
	DDX_Text(pDX, IDC_EDIT_GRIDY, m_gridIndex.y);
}

BEGIN_MESSAGE_MAP(CMineSolverDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_STARTMINE, OnBnClickedButtonStartMinesweeper)
	ON_BN_CLICKED(IDC_BUTTON_CLOSEMINESWEEP, OnBnClickedButtonCloseMinesweeper)
	ON_BN_CLICKED(IDC_BUTTON_GOTOGRID, OnBnClickedButtonGotoGrid)
	ON_WM_MOVE()
	ON_BN_CLICKED(IDC_BUTTON_IDENTIFY, OnBnClickedButtonIdentifySquare)
    ON_BN_CLICKED(IDC_BUTTON_NEWGAME, OnBnClickedButtonNewGame)
    ON_BN_CLICKED(IDC_BUTTON_PLAYGAME, OnBnClickedButtonPlayGame)
END_MESSAGE_MAP()


// CMineSolverDlg message handlers

BOOL CMineSolverDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMineSolverDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMineSolverDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//********************************************************
BOOL CALLBACK EnumWindowsProcFindMine(HWND hwnd, LPARAM lParam)
{
    char windowName[256];
	int nameLength = GetWindowText(hwnd, windowName, 256);
    if (strcmp(windowName, "Minesweeper") == 0)
	{
		g_minesweeperWindow = hwnd;
		return FALSE;  //stop enumeration of windows
	}
	else
		return TRUE;   //continue enumeration of windows, searching for Minesweeper
}

//********************************************************
void CMineSolverDlg::OnBnClickedButtonStartMinesweeper()
{
	::EnumWindows(EnumWindowsProcFindMine, 0);
	if (g_minesweeperWindow != NULL)
	{
		m_ctrlFeedback.SetWindowText("Found existing Minesweeper window, no need to start another one");	
		::ShowWindow(g_minesweeperWindow, SW_SHOWNORMAL);
		::SetForegroundWindow(g_minesweeperWindow);
	}
	else  
	{
		//couldn't find an existing minesweeper window, need to create a new one
		m_ctrlFeedback.SetWindowText("Could not find Minesweeper window, starting another one");
		WinExec("winmine.exe", SW_SHOWNORMAL);
		::EnumWindows(EnumWindowsProcFindMine, 0);
		if (g_minesweeperWindow != NULL)
			m_ctrlFeedback.SetWindowText("Created and linked new Minesweeper window");
		else
		{
			m_ctrlFeedback.SetWindowText("Could not link new Minesweeper window");
			return;
		}
	}

	//Determine the offset between the minesweeper window and the solver window
	//we'll maintain this offset as the solver window is moved
	//Note that the user can always relink the two windows if he wants to change
	//this offset
	RECT solverWindowRect;
	RECT mineWindowRect;
	GetWindowRect(&solverWindowRect);
	::GetWindowRect(g_minesweeperWindow, &mineWindowRect);	
	//m_mineWindowOffset.x = mineWindowRect.left - solverWindowRect.left;
	//m_mineWindowOffset.y = mineWindowRect.top - solverWindowRect.top;
    m_mineWindowOffset.x = solverWindowRect.right - solverWindowRect.left;
	m_mineWindowOffset.y = 0;
    OnMove(solverWindowRect.left, solverWindowRect.top+20);
}

//********************************************************
void CMineSolverDlg::OnBnClickedButtonCloseMinesweeper()
{
	if (g_minesweeperWindow == NULL)
	{
		m_ctrlFeedback.SetWindowText("I don't know about any open Minesweeper Window");
		return;
	}

	//Find location of closebox on minesweeper window
	RECT windowRect;
	::GetWindowRect(g_minesweeperWindow, &windowRect);
	POINT closeBox;
	closeBox.x = windowRect.right - 5;
	closeBox.y = windowRect.top + 5;
	
	//Attempt to Set cursor position to close box
	int success = SetCursorPos(closeBox.x, closeBox.y);
	if (!success)	
	{
		CString message;
		message.Format("Could not click on closebox at (%d, %d)", closeBox.x, closeBox.y);
		m_ctrlFeedback.SetWindowText(message);
		return;
	}

	//click the left mouse button to close the window
	mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 
			0,0,0,0); 

	//clear our global variable to avoid confusion if we later start another
	//minesweeper window
	g_minesweeperWindow = NULL;
}

//********************************************************
void CMineSolverDlg::OnBnClickedButtonGotoGrid()
{
	//error checking
	if (g_minesweeperWindow == NULL)
	{
		m_ctrlFeedback.SetWindowText("I don't know about any open Minesweeper Window");
		return;
	}

	//get the latest grid X,Y values from user
	//stored in m_gridIndex
	UpdateData();

	//determine where the specified point is on the screen
	RECT mineWindowRect;
    POINT point;
    ::GetWindowRect(g_minesweeperWindow, &mineWindowRect);    
	ComputeGridPoint(mineWindowRect, m_gridIndex, point);

	//Attempt to Set cursor position to grid point
	int success = SetCursorPos(point.x, point.y);
	if (!success)	
	{
		CString message;
		message.Format("Could not go to on grid point at (%d, %d)", point.x, point.y);
		m_ctrlFeedback.SetWindowText(message);
		return;
	}
}

//********************************************************
void CMineSolverDlg::ComputeGridPoint(const RECT &mineWindowRect,
									  const POINT &gridIndex, 
									  POINT &gridPoint) const
{
	POINT gridStart;
	gridStart.x = mineWindowRect.left + 23;
	gridStart.y = mineWindowRect.top + 104;

	POINT gridSpacing;
	gridSpacing.x = 16;
	gridSpacing.y = 16;

	gridPoint.x = gridStart.x + gridIndex.x * gridSpacing.x;
	gridPoint.y = gridStart.y + gridIndex.y * gridSpacing.y;
}

//********************************************************
void CMineSolverDlg::OnMove(int x, int y)
{
	//determine the relative position of the Minesweeper window
	//then move the minesweeper window based on this offset
	if (g_minesweeperWindow != NULL)
	{		
		RECT mineWindowRect;
		::GetWindowRect(g_minesweeperWindow, &mineWindowRect);
		::MoveWindow(g_minesweeperWindow, 
				x + m_mineWindowOffset.x,
				y + m_mineWindowOffset.y - 20,  //-20 to account for title bar
				mineWindowRect.right - mineWindowRect.left,
				mineWindowRect.bottom - mineWindowRect.top,
				1);				
	}

	//Move the MineSolver window
	CDialog::OnMove(x, y);
}

//********************************************************
void CMineSolverDlg::OnBnClickedButtonIdentifySquare()
{
	//error checking
	if (g_minesweeperWindow == NULL)
	{
		m_ctrlFeedback.SetWindowText("I don't know about any open Minesweeper Window");
		return;
	}

    //Get the rectangle we're going to capture
	RECT mineWindowRect;
    ::GetWindowRect(g_minesweeperWindow, &mineWindowRect);    

    //setup device context handles
    HDC hMineWindowDC;
    hMineWindowDC = ::GetWindowDC(g_minesweeperWindow);
    
    //get the latest grid X,Y values from user, store them in m_gridIndex
	//then determine where the specified point is on the screen
    UpdateData();
    POINT screenPoint, windowPoint;
    ComputeGridPoint(mineWindowRect, m_gridIndex, screenPoint);
    windowPoint.x = screenPoint.x - mineWindowRect.left;
    windowPoint.y = screenPoint.y - mineWindowRect.top;

    //sample the color of the pixel
    COLORREF sampledColor1, sampledColor2;  //format 0x00bbggrr 
    sampledColor1 = GetPixel(hMineWindowDC, windowPoint.x, windowPoint.y);
    sampledColor2 = GetPixel(hMineWindowDC, windowPoint.x-7, windowPoint.y-7);
    ::ReleaseDC(g_minesweeperWindow, hMineWindowDC); 

    CString message;
    message.Format("Sampled color #1 is %X\r\n", sampledColor1);
    message.AppendFormat("Sampled color #2 is %X\r\n", sampledColor2);
    int numMines = GetNumMines(sampledColor1, sampledColor2);
    message.AppendFormat("There are %d mines next to this square.", numMines);
    m_ctrlFeedback.SetWindowText(message);
}

//********************************************************
int CMineSolverDlg::GetNumMines(COLORREF sampledColor1, COLORREF sampledColor2) const
{
    int numMines = MINE_UNKNOWN;
    if (sampledColor1 == 0x00FF0000)  //light blue
        numMines = 1;
    else if (sampledColor1 == 0x00008200)  //green
        numMines = 2;
    else if (sampledColor1 == 0x000000FF)  //green
        numMines = 3;
    else if (sampledColor1 == 0x00840000)  //dark blue
        numMines = 4;
    else if (sampledColor1 == 0x00000084)  //maroon
        numMines = 5;    
    else if (sampledColor1 == 0x00848200)  //teal
        numMines = 6;    
    else if (sampledColor1 == 0x00C6C3C6)  //gray
    {
        if (sampledColor2 == 0x00FFFFFF)  //white
            numMines = MINE_UNKNOWN;
        else if (sampledColor2 == 0x00C6C3C6)  //gray
            numMines = 0;
    }
    else if (sampledColor1 == 0x00000000)  //black
    {
        if (sampledColor2 == 0x00FFFFFF)  //white
            numMines = MINE_MARKED;
    }
    return numMines;
}

//********************************************************
void CMineSolverDlg::OnBnClickedButtonNewGame()
//assume we're playing the expert-level game
{
	//determine where the specified point is on the screen
	RECT mineWindowRect;
    POINT point;
    ::GetWindowRect(g_minesweeperWindow, &mineWindowRect);    
    point.x = (mineWindowRect.left + mineWindowRect.right)/2;
    point.y = mineWindowRect.top + 70;

	//Attempt to Set cursor position to grid point
	int success = SetCursorPos(point.x, point.y);
	if (!success)	
	{
		CString message;
		message.Format("Could not go to point at (%d, %d)", point.x, point.y);
		m_ctrlFeedback.SetWindowText(message);
		return;
	}
    
	//click the left mouse button once to activate window
	mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 
			0,0,0,0); 
    //click the left mouse button a second time to reset game
    mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 
			0,0,0,0); 
}

//********************************************************
void CMineSolverDlg::ScanMinefield()
{
    assert(g_minesweeperWindow);

    //Get the rectangle we're going to capture
	RECT mineWindowRect;
    ::GetWindowRect(g_minesweeperWindow, &mineWindowRect);    

    //setup device context handles
    HDC hMineWindowDC;
    hMineWindowDC = ::GetWindowDC(g_minesweeperWindow);
    
    POINT gridIndex;
    for (gridIndex.x=0; gridIndex.x < m_minefield.width; gridIndex.x++)
        for (gridIndex.y=0; gridIndex.y < m_minefield.height; gridIndex.y++)
        {
	        //determine where the specified point is on the screen
            POINT screenPoint, windowPoint;
            ComputeGridPoint(mineWindowRect, gridIndex, screenPoint);
            windowPoint.x = screenPoint.x - mineWindowRect.left;
            windowPoint.y = screenPoint.y - mineWindowRect.top;

            //sample the color of the pixel
            COLORREF sampledColor1, sampledColor2;  //format 0x00bbggrr 
            sampledColor1 = GetPixel(hMineWindowDC, windowPoint.x, windowPoint.y);
            sampledColor2 = GetPixel(hMineWindowDC, windowPoint.x-7, windowPoint.y-7);
             
            int numMines = GetNumMines(sampledColor1, sampledColor2);
            m_minefield(gridIndex.x, gridIndex.y) = numMines;
        }

    //cleanup Device context
    ::ReleaseDC(g_minesweeperWindow, hMineWindowDC); 
}

//********************************************************
void CMineSolverDlg::ProcessMineLogic()
//actually, doesn't need to be a member function, but keeping it here for convenience
{
    const int width = m_minefield.width;
    const int height = m_minefield.height;
    for (int x=0; x < width; x++)
        for (int y=0; y < height; y++)
        {
            int numMines = m_minefield(x,y);

            //short circuit if this mine has already been marked
            //or if there's no logic necessary
            if (numMines == MINE_CLEAR || 
                numMines == MINE_MARKED || 
                numMines == 0)
                continue;

            //Get list of neighbors of this mine
            std::vector<CPoint> neighbors;
            //start out getting the 4-connected neighbors
            if (x-1 >= 0) 
                neighbors.push_back( CPoint(x-1, y) );
            if (y-1 >= 0)
                neighbors.push_back( CPoint(x, y-1) );
            if (x+1 < width-1) 
                neighbors.push_back( CPoint(x+1, y) );
            if (y+1 < height-1)
                neighbors.push_back( CPoint(x, y+1) );
            //continue to find the 8-connected neighbors
            if ((x-1 >= 0) && (y-1 >= 0))
                neighbors.push_back( CPoint(x-1, y-1) );
            if ((x-1 >= 0) && (y+1 <= height-1))
                neighbors.push_back( CPoint(x-1, y+1) );                
            if ((x+1 <= width-1) && (y-1 >= 0))
                neighbors.push_back( CPoint(x+1, y-1) );        
            if ((x+1 <= width-1) && (y+1 <= height-1))
                neighbors.push_back( CPoint(x+1, y+1) );
            
            //remove neighbors that are known to be empty
            //yes, I know that there are more efficient ways of doing this, but the data set is so small that it doesn't matter
            const int initNeighbors = (int)neighbors.size();
            std::vector<CPoint>::iterator curNeighbor;
            curNeighbor = neighbors.begin(); 
            while (curNeighbor != neighbors.end())
            {
                if ((m_minefield(curNeighbor->x, curNeighbor->y) == MINE_CLEAR) ||
                    (m_minefield(curNeighbor->x, curNeighbor->y) >= 0))
                {
                    curNeighbor = neighbors.erase(curNeighbor);
                }
                else if (m_minefield(curNeighbor->x, curNeighbor->y) == MINE_MARKED)
                {
                    numMines--;
                    curNeighbor = neighbors.erase(curNeighbor);
                }
                else
                    curNeighbor++;
            }

            //if we've already accounted for all the mines, mark the rest of the neighbors
            //as empty
            if (numMines == 0)
            {
                for (curNeighbor=neighbors.begin(); curNeighbor!=neighbors.end(); curNeighbor++)
                    if (m_minefield(curNeighbor->x, curNeighbor->y) == MINE_UNKNOWN)
                        m_minefield(curNeighbor->x, curNeighbor->y) = MINE_CLEAR;                
            }

            //if we have the same number of mines as neighbors, mark the neighbors as mines
            else if (numMines == neighbors.size())
            {
                for (curNeighbor=neighbors.begin(); curNeighbor!=neighbors.end(); curNeighbor++)
                    m_minefield(curNeighbor->x, curNeighbor->y) = MINE_MARKED;
            }
        }
}

//********************************************************
void CMineSolverDlg::MarkKnownMines()
{
    assert(g_minesweeperWindow);
    RECT mineWindowRect;
    ::GetWindowRect(g_minesweeperWindow, &mineWindowRect);   

    //setup device context handles
    HDC hMineWindowDC;
    hMineWindowDC = ::GetWindowDC(g_minesweeperWindow);

    //click the title bar to make sure the minesweeper window is active
    POINT titleBarPoint;
    titleBarPoint.x = (mineWindowRect.left + mineWindowRect.right)/2;
    titleBarPoint.y = mineWindowRect.top + 10;
    SetCursorPos(titleBarPoint.x, titleBarPoint.y);
    mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 
        0,0,0,0); 
    
    //bug in minesweeper? it ignores my first right click!
    POINT anyGridIndex;   anyGridIndex.x = 2;  anyGridIndex.y = 3;  
    POINT anyPointOnGrid;
    ComputeGridPoint(mineWindowRect, anyGridIndex, anyPointOnGrid);
    SetCursorPos(anyPointOnGrid.x, anyPointOnGrid.y);
    mouse_event(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP, 
        0,0,0,0); 

    POINT gridIndex;
    for (gridIndex.x=0; gridIndex.x < m_minefield.width; gridIndex.x++)
        for (gridIndex.y=0; gridIndex.y < m_minefield.height; gridIndex.y++)
        {
            if (m_minefield(gridIndex.x,gridIndex.y) == MINE_MARKED)
            {
                //determine where the specified point is on the screen
                POINT screenPoint, windowPoint;
                ComputeGridPoint(mineWindowRect, gridIndex, screenPoint);
                windowPoint.x = screenPoint.x - mineWindowRect.left;
                windowPoint.y = screenPoint.y - mineWindowRect.top;

                //sample the color of the pixel
                COLORREF sampledColor1, sampledColor2;  //format 0x00bbggrr 
                sampledColor1 = GetPixel(hMineWindowDC, windowPoint.x, windowPoint.y);
                sampledColor2 = GetPixel(hMineWindowDC, windowPoint.x-7, windowPoint.y-7);

                //test whether the mine was already marked
                int numMines = GetNumMines(sampledColor1, sampledColor2);                
                assert (numMines == MINE_MARKED || numMines == MINE_UNKNOWN);  //we've made a logic error if we're trying to mark a mine that's got a number in it
                if (numMines == MINE_UNKNOWN)
                {
	                //Attempt to Set cursor position to grid point
	                int success = SetCursorPos(screenPoint.x, screenPoint.y);
                    assert(success);

	                //click the right mouse button to tag the mine
	                mouse_event(MOUSEEVENTF_RIGHTDOWN | MOUSEEVENTF_RIGHTUP, 
			                0,0,0,0); 
                }
            }
        }
       
    //cleanup Device context
    ::ReleaseDC(g_minesweeperWindow, hMineWindowDC); 

    //Set cursor to a safe point to avoid accidental clicks
    SetCursorPos(titleBarPoint.x, titleBarPoint.y);
}

//********************************************************
void CMineSolverDlg::MakeNextMove()
{
    //find the last cell that needs to be cleared
    bool foundNextPoint = false;
    POINT gridIndex;    
    for (int x=0; x < m_minefield.width; x++)
        for (int y=0; y < m_minefield.height; y++)
        {
            if (m_minefield(x,y) == MINE_CLEAR)
            {
                foundNextPoint = true;
                gridIndex.x = x;
                gridIndex.y = y;
            }                
        }

    //if we're out of moves, there's nothing the computer can do
    if (foundNextPoint == false)
        return;

    //determine where the specified point is on the screen
    assert(g_minesweeperWindow);
    RECT mineWindowRect;
    ::GetWindowRect(g_minesweeperWindow, &mineWindowRect);   
    POINT screenPoint;
    ComputeGridPoint(mineWindowRect, gridIndex, screenPoint);

	//Attempt to Set cursor position to grid point
	int success = SetCursorPos(screenPoint.x, screenPoint.y);
    assert(success);

	//click the right mouse button to tag the mine
    mouse_event(MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, 
			0,0,0,0); 



    //TODO
    //need to add random guessing
    //need to check and see result of click (e.g., hit a mine)



}

//********************************************************
void CMineSolverDlg::OnBnClickedButtonPlayGame()
//find up to N mines (limit to N mines to avoid infinite loop when my logic is bad
{
    for (int move=0; move<20; move++)
    {
        ScanMinefield();
        ProcessMineLogic();
        MarkKnownMines();
        MakeNextMove();
    } 
}
