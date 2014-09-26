--TEST--
Bug #33318 (throw 1; results in Invalid opcode 108/1/8)
--FILE--
<?php
throw 1;
?>
--EXPECTF--
Fatal error: Uncaught exception 'EngineException' with message 'Can only throw objects' in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
