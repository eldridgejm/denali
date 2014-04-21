"""""""""""""""""""""""""""""""
" denali vim project settings "
"""""""""""""""""""""""""""""""
set wildignore+=*/build/*,*/extern/*,*/doc/dev/latex/*,*/doc/dev/html/*,*.orig*,*.pyc
set wildignore+=*/doc/pydoc/_build/*,*/doc/pages/*
noremap <F5> :!ctags -R --exclude=build --exclude=extern --exclude=doc<CR>
