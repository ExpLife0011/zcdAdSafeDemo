

#pragma once



class BrowserCheck
{
public:
  BrowserCheck();
  ~BrowserCheck();
  int check();

  int checkPlatformName();
  int chekcPlatformType();
  int checkPlatformVersion();
  int checkBrowserName();
  int checkBrowserType();
  int checkBrowserVersion();
};
