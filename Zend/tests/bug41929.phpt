--TEST--
Bug #41929 (Foreach on object does not iterate over all visible properties)
--FILE--
<?php

class C
{
    private $priv = "ok";
    function doLoop()
    {
        echo $this->priv, "\n";
        foreach ($this as $k => $v) {
            echo "{$k}: {$v}\n";
        }
    }
}
class D extends C
{
}
function fn1611238083()
{
    $myD = new D();
    $myD->doLoop();
}
fn1611238083();
--EXPECT--
ok
priv: ok
