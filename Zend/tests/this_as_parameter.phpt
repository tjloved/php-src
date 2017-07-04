--TEST--
$this as parameter
--FILE--
<?php

function foo($this)
{
    var_dump($this);
}
function fn467663257()
{
    foo(5);
}
fn467663257();
--EXPECTF--
Fatal error: Cannot use $this as parameter in %sthis_as_parameter.php on line %d
