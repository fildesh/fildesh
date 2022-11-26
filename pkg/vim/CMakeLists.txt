
install(FILES ftdetect/fildesh.vim
  COMPONENT "share_vim"
  DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/vim/vimfiles/ftdetect/")
install(FILES ftplugin/fildesh.vim
  COMPONENT "share_vim"
  DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/vim/vimfiles/ftplugin/")
install(FILES indent/fildesh.vim
  COMPONENT "share_vim"
  DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/vim/vimfiles/indent/")
install(FILES syntax/fildesh.vim
  COMPONENT "share_vim"
  DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/vim/vimfiles/syntax/")

