// MineSolverDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "Grid2D.h"

#define MINEFIELD_WIDTH  30
#define MINEFIELD_HEIGHT 16
#define MINE_UNKNOWN     -1
#define MINE_MARKED      -2
#define MINE_CLEAR       -3

// CMineSolverDlg dialog
class CMineSolverDlg : public CDialog
{
// Construction
public:
	CMineSolverDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MINESOLVER_DIALOG };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedButtonStartMinesweeper();
	afx_msg void OnBnClickedButtonCloseMinesweeper();
	afx_msg void OnBnClickedButtonGotoGrid();
	afx_msg void OnBnClickedButtonIdentifySquare();
	afx_msg void OnMove(int x, int y);
    afx_msg void OnBnClickedButtonNewGame();
    afx_msg void OnBnClickedButtonPlayGame();

	//Control Variables
	CEdit m_ctrlFeedback;
	CEdit m_ctrlGridX;
	CEdit m_ctrlGridY;
	POINT m_gridIndex;
	
	//hidden - just for implementation
private:
	POINT m_mineWindowOffset;
	void ComputeGridPoint(const RECT &mineWindowRect,
						  const POINT &gridIndex,
						  POINT &gridPoint) const;
    int GetNumMines(COLORREF sampledColor1, COLORREF sampledColor2) const;
    void ScanMinefield();
    void ProcessMineLogic();
    void MarkKnownMines();
    void MakeNextMove();
    Grid2D<int> m_minefield;  //number of mines at each position
};

//Global function necessary for finding the minesweeper window
BOOL CALLBACK EnumWindowsProcFindMine(HWND hwnd, LPARAM lParam);