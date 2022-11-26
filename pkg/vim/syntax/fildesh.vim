" Vim filetype file
" Language:     Fildesh
" Author:       Alex Klinkhamer (thru.fildesh@grencez.dev)
" URL:          https://github.com/fildesh/fildesh

if exists("b:current_syntax")
  finish
endif

syn case match

syn match   fildeshComment /#.*$/
syn match   fildeshInclude /\$(<< [^)]*)/
syn match   fildeshKeyword "$(barrier)"
syn region  fildeshString start=/"/ skip=/\\"/ end=/"/

hi def link fildeshComment Comment
hi def link fildeshInclude Include
hi def link fildeshKeyword Keyword
hi def link fildeshString String

let b:current_syntax = "fildesh"
