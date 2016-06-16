--TEST--
Return type of self is allowed in closure

--FILE--
<?php

class Bar
{
}
function fn1389195412()
{
    $c = function () : self {
        return $this;
    };
    var_dump($c->call(new Bar()));
}
fn1389195412();
--EXPECT--
object(Bar)#2 (0) {
}
