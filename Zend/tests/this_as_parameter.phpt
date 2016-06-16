--TEST--
$this as parameter
--FILE--
<?php

function foo($this)
{
    var_dump($this);
}
function fn769705290()
{
    foo(5);
}
fn769705290();
--EXPECTF--
Fatal error: Cannot use $this as parameter in %sthis_as_parameter.php on line %d
