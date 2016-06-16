--TEST--
$this in eval() block
--FILE--
<?php

class C
{
    function foo()
    {
        eval('var_dump($this);');
        eval('var_dump($this);');
    }
}
function fn658177768()
{
    $x = new C();
    $x->foo();
}
fn658177768();
--EXPECT--
object(C)#1 (0) {
}
object(C)#1 (0) {
}

