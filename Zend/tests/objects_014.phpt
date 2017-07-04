--TEST--
extending the same interface twice
--FILE--
<?php

interface foo
{
}
interface bar extends foo, foo
{
}
function fn2059271922()
{
    echo "Done\n";
}
fn2059271922();
--EXPECTF--	
Fatal error: Class bar cannot implement previously implemented interface foo in %s on line %d
