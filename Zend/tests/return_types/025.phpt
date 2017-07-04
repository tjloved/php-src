--TEST--
Return type of self is allowed in closure

--FILE--
<?php

class Bar
{
}
function fn141482475()
{
    $c = function () : self {
        return $this;
    };
    var_dump($c->call(new Bar()));
}
fn141482475();
--EXPECT--
object(Bar)#2 (0) {
}
