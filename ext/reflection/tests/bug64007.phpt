--TEST--
Bug #64007 (There is an ability to create instance of Generator by hand)
--FILE--
<?php
$reflection = new ReflectionClass('Generator');
try {
    $generator = $reflection->newInstanceWithoutConstructor();
    var_dump($generator);
} catch (Exception $e) {
    var_dump($e->getMessage());
}

$generator  = $reflection->newInstance();
var_dump($generator);
?>
--EXPECTF--
string(97) "Class Generator is an internal class that cannot be instantiated without invoking its constructor"

Fatal error: Uncaught exception 'EngineException' with message 'The "Generator" class is reserved for internal use and cannot be manually instantiated' in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
