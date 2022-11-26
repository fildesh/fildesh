" Vim indent file
" Language:     Fildesh
" Author:       Alex Klinkhamer (thru.fildesh@grencez.dev)
" URL:          https://github.com/fildesh/fildesh

" Only load this indent file when no other was loaded.
if exists("b:did_indent")
  finish
endif
let b:did_indent = 1

setlocal indentexpr=GetFildeshIndent()
setlocal indentkeys=0{,0},0),0],:,0#,!^F,o,O,e

" Only define the function once.
if exists("*GetFildeshIndent")
  finish
endif

let s:cpo_save = &cpo
set cpo&vim


function GetFildeshIndent()
  " Get current line
  let line = getline(v:lnum)
  if v:lnum == 0
    return 0
  endif

  let pline = getline(v:lnum - 1)

  if pline =~ '\\$'
    return 2
  endif
  return 0
endfunction


let &cpo = s:cpo_save
unlet s:cpo_save
