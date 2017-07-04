--TEST--
implementing a class
--FILE--
<?php

class foo
{
}
interface bar extends foo
{
}
function fn1983134205()
{
    echo "Done\n";
}
fn1983134205();
--EXPECTF--	
Fatal error: bar cannot implement foo - it is not an interface in %s on line %d
