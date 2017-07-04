--TEST--
Bug #37707 (clone without assigning leaks memory)
--FILE--
<?php

class testme
{
    function __clone()
    {
        echo "clonned\n";
    }
}
function fn2001993225()
{
    clone new testme();
    echo "NO LEAK\n";
}
fn2001993225();
--EXPECT--
clonned
NO LEAK

