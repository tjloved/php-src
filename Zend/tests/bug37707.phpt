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
function fn1256797162()
{
    clone new testme();
    echo "NO LEAK\n";
}
fn1256797162();
--EXPECT--
clonned
NO LEAK

