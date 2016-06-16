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
function fn594482669()
{
    echo "Done\n";
}
fn594482669();
--EXPECTF--	
Fatal error: Class bar cannot implement previously implemented interface foo in %s on line %d
