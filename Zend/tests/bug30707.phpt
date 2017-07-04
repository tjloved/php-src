--TEST--
Bug #30707 (Segmentation fault on exception in method)
--FILE--
<?php

class C
{
    function byePHP($plop)
    {
        echo "ok\n";
    }
    function plip()
    {
        try {
            $this->plap($this->plop());
        } catch (Exception $e) {
        }
    }
    function plap($a)
    {
    }
    function plop()
    {
        throw new Exception();
    }
}
function fn2122881604()
{
    $x = new C();
    $x->byePHP($x->plip());
}
fn2122881604();
--EXPECT--
ok
