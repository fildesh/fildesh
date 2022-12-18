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

  let pnum = prevnonblank(v:lnum - 1)
  if pnum == 0
    return 0
  endif
  let ppline = getline(pnum - 1)
  if ppline =~ '\\$'
    return 0
  endif

  let bnum = pnum
  let parencol = 0
  let nparens = 0

  while bnum > 0
    let bline = getline(bnum)
    if bline =~ '^\s*#.*$'
      let bline = ''
    endif
    let col = strlen(bline)
    for c in reverse(split(bline, '\zs'))
      if c == '('
        if nparens == 0
          let parencol = col
          break
        endif
        let nparens = nparens - 1
      elseif c == ')'
        let nparens = nparens + 1
      endif
      let col = col - 1
    endfor
    unlet col

    if parencol > 0
      break
    endif
    if nparens == 0
      return indent(bnum)
    endif

    let bnum = prevnonblank(bnum - 1)
  endwhile
  unlet nparens

  if parencol > 0
    return parencol
  endif

  return indent(pnum)
endfunction


let &cpo = s:cpo_save
unlet s:cpo_save
