"""""""""""""""""""""""""""""""
" denali vim project settings "
"""""""""""""""""""""""""""""""
set wildignore+=*/build/*,*/extern/*,*/doc/latex/*,*/doc/html/*,*.orig*
set path=/home/eldridge/workbench/denali/code/**
noremap <F5> :!ctags -R --exclude=build --exclude=extern --exclude=doc<CR>
