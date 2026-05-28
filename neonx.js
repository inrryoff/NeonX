// include: shell.js
// include: minimum_runtime_check.js
// end include: minimum_runtime_check.js
// The Module object: Our interface to the outside world. We import
// and export values on it. There are various ways Module can be used:
// 1. Not defined. We create it here
// 2. A function parameter, function(moduleArg) => Promise<Module>
// 3. pre-run appended it, var Module = {}; ..generated code..
// 4. External script tag defines var Module.
// We need to check if Module already exists (e.g. case 3 above).
// Substitution will be replaced with actual code on later stage of the build,
// this way Closure Compiler will not mangle it (e.g. case 4. above).
// Note that if you want to run closure, and also to use Module
// after the generated code, you will need to define   var Module = {};
// before the code. Then that object will be used in the code, and you
// can continue to use Module afterwards as well.
var Module = typeof Module != 'undefined' ? Module : {};

// Determine the runtime environment we are in. You can customize this by
// setting the ENVIRONMENT setting at compile time (see settings.js).

// Attempt to auto-detect the environment
var ENVIRONMENT_IS_WEB = !!globalThis.window;
var ENVIRONMENT_IS_WORKER = !!globalThis.WorkerGlobalScope;
// N.b. Electron.js environment is simultaneously a NODE-environment, but
// also a web environment.
var ENVIRONMENT_IS_NODE = globalThis.process?.versions?.node && globalThis.process?.type != 'renderer';
var ENVIRONMENT_IS_SHELL = !ENVIRONMENT_IS_WEB && !ENVIRONMENT_IS_NODE && !ENVIRONMENT_IS_WORKER;

// --pre-jses are emitted after the Module integration code, so that they can
// refer to Module (if they choose; they can also define Module)


var arguments_ = [];
var thisProgram = './this.program';
var quit_ = (status, toThrow) => {
  throw toThrow;
};

// In MODULARIZE mode _scriptName needs to be captured already at the very top of the page immediately when the page is parsed, so it is generated there
// before the page load. In non-MODULARIZE modes generate it here.
var _scriptName = globalThis.document?.currentScript?.src;

if (typeof __filename != 'undefined') { // Node
  _scriptName = __filename;
} else
if (ENVIRONMENT_IS_WORKER) {
  _scriptName = self.location.href;
}

// `/` should be present at the end if `scriptDirectory` is not empty
var scriptDirectory = '';
function locateFile(path) {
  if (Module['locateFile']) {
    return Module['locateFile'](path, scriptDirectory);
  }
  return scriptDirectory + path;
}

// Hooks that are implemented differently in different runtime environments.
var readAsync, readBinary;

if (ENVIRONMENT_IS_NODE) {

  // These modules will usually be used on Node.js. Load them eagerly to avoid
  // the complexity of lazy-loading.
  var fs = require('node:fs');

  scriptDirectory = __dirname + '/';

// include: node_shell_read.js
readBinary = (filename) => {
  // We need to re-wrap `file://` strings to URLs.
  filename = isFileURI(filename) ? new URL(filename) : filename;
  var ret = fs.readFileSync(filename);
  return ret;
};

readAsync = async (filename, binary = true) => {
  // See the comment in the `readBinary` function.
  filename = isFileURI(filename) ? new URL(filename) : filename;
  var ret = fs.readFileSync(filename, binary ? undefined : 'utf8');
  return ret;
};
// end include: node_shell_read.js
  if (process.argv.length > 1) {
    thisProgram = process.argv[1].replace(/\\/g, '/');
  }

  arguments_ = process.argv.slice(2);

  // MODULARIZE will export the module in the proper place outside, we don't need to export here
  if (typeof module != 'undefined') {
    module['exports'] = Module;
  }

  quit_ = (status, toThrow) => {
    process.exitCode = status;
    throw toThrow;
  };

} else

// Note that this includes Node.js workers when relevant (pthreads is enabled).
// Node.js workers are detected as a combination of ENVIRONMENT_IS_WORKER and
// ENVIRONMENT_IS_NODE.
if (ENVIRONMENT_IS_WEB || ENVIRONMENT_IS_WORKER) {
  try {
    scriptDirectory = new URL('.', _scriptName).href; // includes trailing slash
  } catch {
    // Must be a `blob:` or `data:` URL (e.g. `blob:http://site.com/etc/etc`), we cannot
    // infer anything from them.
  }

  {
// include: web_or_worker_shell_read.js
if (ENVIRONMENT_IS_WORKER) {
    readBinary = (url) => {
      var xhr = new XMLHttpRequest();
      xhr.open('GET', url, false);
      xhr.responseType = 'arraybuffer';
      xhr.send(null);
      return new Uint8Array(/** @type{!ArrayBuffer} */(xhr.response));
    };
  }

  readAsync = async (url) => {
    // Fetch has some additional restrictions over XHR, like it can't be used on a file:// url.
    // See https://github.com/github/fetch/pull/92#issuecomment-140665932
    // Cordova or Electron apps are typically loaded from a file:// url.
    // So use XHR on webview if URL is a file URL.
    if (isFileURI(url)) {
      return new Promise((resolve, reject) => {
        var xhr = new XMLHttpRequest();
        xhr.open('GET', url, true);
        xhr.responseType = 'arraybuffer';
        xhr.onload = () => {
          if (xhr.status == 200 || (xhr.status == 0 && xhr.response)) { // file URLs can return 0
            resolve(xhr.response);
            return;
          }
          reject(xhr.status);
        };
        xhr.onerror = reject;
        xhr.send(null);
      });
    }
    var response = await fetch(url, { credentials: 'same-origin' });
    if (response.ok) {
      return response.arrayBuffer();
    }
    throw new Error(response.status + ' : ' + response.url);
  };
// end include: web_or_worker_shell_read.js
  }
} else
{
}

var out = console.log.bind(console);
var err = console.error.bind(console);

// end include: shell.js

// include: preamble.js
// === Preamble library stuff ===

// Documentation for the public APIs defined in this file must be updated in:
//    site/source/docs/api_reference/preamble.js.rst
// A prebuilt local version of the documentation is available at:
//    site/build/text/docs/api_reference/preamble.js.txt
// You can also build docs locally as HTML or other formats in site/
// An online HTML version (which may be of a different version of Emscripten)
//    is up at http://kripken.github.io/emscripten-site/docs/api_reference/preamble.js.html

var wasmBinary;

// Wasm globals

//========================================
// Runtime essentials
//========================================

// whether we are quitting the application. no code should run after this.
// set in exit() and abort()
var ABORT = false;

// set by exit() and abort().  Passed to 'onExit' handler.
// NOTE: This is also used as the process return code in shell environments
// but only when noExitRuntime is false.
var EXITSTATUS;

// In STRICT mode, we only define assert() when ASSERTIONS is set.  i.e. we
// don't define it at all in release modes.  This matches the behaviour of
// MINIMAL_RUNTIME.
// TODO(sbc): Make this the default even without STRICT enabled.
/** @type {function(*, string=)} */
function assert(condition, text) {
  if (!condition) {
    // This build was created without ASSERTIONS defined.  `assert()` should not
    // ever be called in this configuration but in case there are callers in
    // the wild leave this simple abort() implementation here for now.
    abort(text);
  }
}

/**
 * Indicates whether filename is delivered via file protocol (as opposed to http/https)
 * @noinline
 */
var isFileURI = (filename) => filename.startsWith('file://');

// include: runtime_common.js
// include: runtime_stack_check.js
// end include: runtime_stack_check.js
// include: runtime_exceptions.js
// Base Emscripten EH error class
class EmscriptenEH {}

class EmscriptenSjLj extends EmscriptenEH {}

// end include: runtime_exceptions.js
// include: runtime_debug.js
// end include: runtime_debug.js
// Memory management

var runtimeInitialized = false;



function updateMemoryViews() {
  var b = wasmMemory.buffer;
  HEAP8 = new Int8Array(b);
  HEAP16 = new Int16Array(b);
  HEAPU8 = new Uint8Array(b);
  HEAPU16 = new Uint16Array(b);
  HEAP32 = new Int32Array(b);
  HEAPU32 = new Uint32Array(b);
  HEAPF32 = new Float32Array(b);
  HEAPF64 = new Float64Array(b);
  HEAP64 = new BigInt64Array(b);
  HEAPU64 = new BigUint64Array(b);
}

// include: memoryprofiler.js
// end include: memoryprofiler.js
// end include: runtime_common.js
function preRun() {
  if (Module['preRun']) {
    if (typeof Module['preRun'] == 'function') Module['preRun'] = [Module['preRun']];
    while (Module['preRun'].length) {
      addOnPreRun(Module['preRun'].shift());
    }
  }
  // Begin ATPRERUNS hooks
  callRuntimeCallbacks(onPreRuns);
  // End ATPRERUNS hooks
}

function initRuntime() {
  runtimeInitialized = true;

  // Begin ATINITS hooks
  if (!Module['noFSInit'] && !FS.initialized) FS.init();
TTY.init();
  // End ATINITS hooks

  wasmExports['__wasm_call_ctors']();

  // Begin ATPOSTCTORS hooks
  FS.ignorePermissions = false;
  // End ATPOSTCTORS hooks
}

function postRun() {
   // PThreads reuse the runtime from the main thread.

  if (Module['postRun']) {
    if (typeof Module['postRun'] == 'function') Module['postRun'] = [Module['postRun']];
    while (Module['postRun'].length) {
      addOnPostRun(Module['postRun'].shift());
    }
  }

  // Begin ATPOSTRUNS hooks
  callRuntimeCallbacks(onPostRuns);
  // End ATPOSTRUNS hooks
}

/**
 * @param {string|number=} what
 */
function abort(what) {
  Module['onAbort']?.(what);

  what = `Aborted(${what})`;
  // TODO(sbc): Should we remove printing and leave it up to whoever
  // catches the exception?
  err(what);

  ABORT = true;

  what += '. Build with -sASSERTIONS for more info.';

  // Use a wasm runtime error, because a JS error might be seen as a foreign
  // exception, which means we'd run destructors on it. We need the error to
  // simply make the program stop.
  // FIXME This approach does not work in Wasm EH because it currently does not assume
  // all RuntimeErrors are from traps; it decides whether a RuntimeError is from
  // a trap or not based on a hidden field within the object. So at the moment
  // we don't have a way of throwing a wasm trap from JS. TODO Make a JS API that
  // allows this in the wasm spec.

  // Suppress closure compiler warning here. Closure compiler's builtin extern
  // definition for WebAssembly.RuntimeError claims it takes no arguments even
  // though it can.
  // TODO(https://github.com/google/closure-compiler/pull/3913): Remove if/when upstream closure gets fixed.
  /** @suppress {checkTypes} */
  var e = new WebAssembly.RuntimeError(what);

  // Throw the error whether or not MODULARIZE is set because abort is used
  // in code paths apart from instantiation where an exception is expected
  // to be thrown when abort is called.
  throw e;
}

var wasmBinaryFile;

function findWasmBinary() {
  return locateFile('neonx.wasm');
}

function getBinarySync(file) {
  if (file == wasmBinaryFile && wasmBinary) {
    return new Uint8Array(wasmBinary);
  }
  if (readBinary) {
    return readBinary(file);
  }
  // Throwing a plain string here, even though it not normally advisable since
  // this gets turning into an `abort` in instantiateArrayBuffer.
  throw 'both async and sync fetching of the wasm failed';
}

async function getWasmBinary(binaryFile) {
  // If we don't have the binary yet, load it asynchronously using readAsync.
  if (!wasmBinary) {
    // Fetch the binary using readAsync
    try {
      var response = await readAsync(binaryFile);
      return new Uint8Array(response);
    } catch {
      // Fall back to getBinarySync below;
    }
  }

  // Otherwise, getBinarySync should be able to get it synchronously
  return getBinarySync(binaryFile);
}

async function instantiateArrayBuffer(binaryFile, imports) {
  try {
    var binary = await getWasmBinary(binaryFile);
    var instance = await WebAssembly.instantiate(binary, imports);
    return instance;
  } catch (reason) {
    err(`failed to asynchronously prepare wasm: ${reason}`);

    abort(reason);
  }
}

async function instantiateAsync(binary, binaryFile, imports) {
  if (!binary
      // Don't use streaming for file:// delivered objects in a webview, fetch them synchronously.
      && !isFileURI(binaryFile)
      // Avoid instantiateStreaming() on Node.js environment for now, as while
      // Node.js v18.1.0 implements it, it does not have a full fetch()
      // implementation yet.
      //
      // Reference:
      //   https://github.com/emscripten-core/emscripten/pull/16917
      && !ENVIRONMENT_IS_NODE
     ) {
    try {
      var response = fetch(binaryFile, { credentials: 'same-origin' });
      var instantiationResult = await WebAssembly.instantiateStreaming(response, imports);
      return instantiationResult;
    } catch (reason) {
      // We expect the most common failure cause to be a bad MIME type for the binary,
      // in which case falling back to ArrayBuffer instantiation should work.
      err(`wasm streaming compile failed: ${reason}`);
      err('falling back to ArrayBuffer instantiation');
      // fall back of instantiateArrayBuffer below
    };
  }
  return instantiateArrayBuffer(binaryFile, imports);
}

function getWasmImports() {
  // prepare imports
  var imports = {
    'env': wasmImports,
    'wasi_snapshot_preview1': wasmImports,
  };
  return imports;
}

// Create the wasm instance.
// Receives the wasm imports, returns the exports.
async function createWasm() {
  // Load the wasm module and create an instance of using native support in the JS engine.
  // handle a generated wasm instance, receiving its exports and
  // performing other necessary setup
  /** @param {WebAssembly.Module=} module*/
  function receiveInstance(instance, module) {
    wasmExports = instance.exports;

    assignWasmExports(wasmExports);

    updateMemoryViews();

    removeRunDependency('wasm-instantiate');
    return wasmExports;
  }
  addRunDependency('wasm-instantiate');

  // Prefer streaming instantiation if available.
  function receiveInstantiationResult(result) {
    // 'result' is a ResultObject object which has both the module and instance.
    // receiveInstance() will swap in the exports (to Module.asm) so they can be called
    // TODO: Due to Closure regression https://github.com/google/closure-compiler/issues/3193, the above line no longer optimizes out down to the following line.
    // When the regression is fixed, can restore the above PTHREADS-enabled path.
    return receiveInstance(result['instance']);
  }

  var info = getWasmImports();

  // User shell pages can write their own Module.instantiateWasm = function(imports, successCallback) callback
  // to manually instantiate the Wasm module themselves. This allows pages to
  // run the instantiation parallel to any other async startup actions they are
  // performing.
  // Also pthreads and wasm workers initialize the wasm instance through this
  // path.
  if (Module['instantiateWasm']) {
    return new Promise((resolve, reject) => {
        Module['instantiateWasm'](info, (inst, mod) => {
          resolve(receiveInstance(inst, mod));
        });
    });
  }

  wasmBinaryFile ??= findWasmBinary();
  var result = await instantiateAsync(wasmBinary, wasmBinaryFile, info);
  var exports = receiveInstantiationResult(result);
  return exports;
}

// end include: preamble.js

// Begin JS library code


  class ExitStatus {
      name = 'ExitStatus';
      constructor(status) {
        this.message = `Program terminated with exit(${status})`;
        this.status = status;
      }
    }

  /** @type {!Int16Array} */
  var HEAP16;

  /** @type {!Int32Array} */
  var HEAP32;

  /** not-@type {!BigInt64Array} */
  var HEAP64;

  /** @type {!Int8Array} */
  var HEAP8;

  /** @type {!Float32Array} */
  var HEAPF32;

  /** @type {!Float64Array} */
  var HEAPF64;

  /** @type {!Uint16Array} */
  var HEAPU16;

  /** @type {!Uint32Array} */
  var HEAPU32;

  /** not-@type {!BigUint64Array} */
  var HEAPU64;

  /** @type {!Uint8Array} */
  var HEAPU8;

  var callRuntimeCallbacks = (callbacks) => {
      while (callbacks.length > 0) {
        // Pass the module as the first argument.
        callbacks.shift()(Module);
      }
    };
  var onPostRuns = [];
  var addOnPostRun = (cb) => onPostRuns.push(cb);

  var onPreRuns = [];
  var addOnPreRun = (cb) => onPreRuns.push(cb);

  var runDependencies = 0;
  
  
  var dependenciesFulfilled = null;
  var removeRunDependency = (id) => {
      runDependencies--;
  
      Module['monitorRunDependencies']?.(runDependencies);
  
      if (runDependencies == 0) {
        if (dependenciesFulfilled) {
          var callback = dependenciesFulfilled;
          dependenciesFulfilled = null;
          callback(); // can add another dependenciesFulfilled
        }
      }
    };
  var addRunDependency = (id) => {
      runDependencies++;
  
      Module['monitorRunDependencies']?.(runDependencies);
  
    };


  
    /**
   * @param {number} ptr
   * @param {string} type
   */
  function getValue(ptr, type = 'i8') {
    if (type.endsWith('*')) type = '*';
    switch (type) {
      case 'i1': return HEAP8[ptr];
      case 'i8': return HEAP8[ptr];
      case 'i16': return HEAP16[((ptr)>>1)];
      case 'i32': return HEAP32[((ptr)>>2)];
      case 'i64': return HEAP64[((ptr)>>3)];
      case 'float': return HEAPF32[((ptr)>>2)];
      case 'double': return HEAPF64[((ptr)>>3)];
      case '*': return HEAPU32[((ptr)>>2)];
      default: abort(`invalid type for getValue: ${type}`);
    }
  }

  var noExitRuntime = true;


  
    /**
   * @param {number} ptr
   * @param {number} value
   * @param {string} type
   */
  function setValue(ptr, value, type = 'i8') {
    if (type.endsWith('*')) type = '*';
    switch (type) {
      case 'i1': HEAP8[ptr] = value; break;
      case 'i8': HEAP8[ptr] = value; break;
      case 'i16': HEAP16[((ptr)>>1)] = value; break;
      case 'i32': HEAP32[((ptr)>>2)] = value; break;
      case 'i64': HEAP64[((ptr)>>3)] = BigInt(value); break;
      case 'float': HEAPF32[((ptr)>>2)] = value; break;
      case 'double': HEAPF64[((ptr)>>3)] = value; break;
      case '*': HEAPU32[((ptr)>>2)] = value; break;
      default: abort(`invalid type for setValue: ${type}`);
    }
  }

  var stackRestore = (val) => __emscripten_stack_restore(val);

  var stackSave = () => _emscripten_stack_get_current();

  

  var wasmTableMirror = [];
  
  
  var getWasmTableEntry = (funcPtr) => {
      var func = wasmTableMirror[funcPtr];
      if (!func) {
        /** @suppress {checkTypes} */
        wasmTableMirror[funcPtr] = func = wasmTable.get(funcPtr);
      }
      return func;
    };
  var ___call_sighandler = (fp, sig) => getWasmTableEntry(fp)(sig);

  var syscallGetVarargI = () => {
      // the `+` prepended here is necessary to convince the JSCompiler that varargs is indeed a number.
      var ret = HEAP32[((+SYSCALLS.varargs)>>2)];
      SYSCALLS.varargs += 4;
      return ret;
    };
  var syscallGetVarargP = syscallGetVarargI;
  
  
  var PATH = {
  isAbs:(path) => path.charAt(0) === '/',
  splitPath:(filename) => {
        var splitPathRe = /^(\/?|)([\s\S]*?)((?:\.{1,2}|[^\/]+?|)(\.[^.\/]*|))(?:[\/]*)$/;
        return splitPathRe.exec(filename).slice(1);
      },
  normalizeArray:(parts, allowAboveRoot) => {
        // if the path tries to go above the root, `up` ends up > 0
        var up = 0;
        for (var i = parts.length - 1; i >= 0; i--) {
          var last = parts[i];
          if (last === '.') {
            parts.splice(i, 1);
          } else if (last === '..') {
            parts.splice(i, 1);
            up++;
          } else if (up) {
            parts.splice(i, 1);
            up--;
          }
        }
        // if the path is allowed to go above the root, restore leading ..s
        if (allowAboveRoot) {
          for (; up; up--) {
            parts.unshift('..');
          }
        }
        return parts;
      },
  normalize:(path) => {
        var isAbsolute = PATH.isAbs(path),
            trailingSlash = path.slice(-1) === '/';
        // Normalize the path
        path = PATH.normalizeArray(path.split('/').filter((p) => !!p), !isAbsolute).join('/');
        if (!path && !isAbsolute) {
          path = '.';
        }
        if (path && trailingSlash) {
          path += '/';
        }
        return (isAbsolute ? '/' : '') + path;
      },
  dirname:(path) => {
        var result = PATH.splitPath(path),
            root = result[0],
            dir = result[1];
        if (!root && !dir) {
          // No dirname whatsoever
          return '.';
        }
        if (dir) {
          // It has a dirname, strip trailing slash
          dir = dir.slice(0, -1);
        }
        return root + dir;
      },
  basename:(path) => path && path.match(/([^\/]+|\/)\/*$/)[1],
join:(...paths) => PATH.normalize(paths.join('/')),
join2:(l, r) => PATH.normalize(l + '/' + r),
};

var initRandomFill = () => {
    // This block is not needed on v19+ since crypto.getRandomValues is builtin
    if (ENVIRONMENT_IS_NODE) {
      var nodeCrypto = require('node:crypto');
      return (view) => nodeCrypto.randomFillSync(view);
    }

    return (view) => (crypto.getRandomValues(view), 0);
  };
var randomFill = (view) => (randomFill = initRandomFill())(view);



var PATH_FS = {
resolve:(...args) => {
      var resolvedPath = '',
        resolvedAbsolute = false;
      for (var i = args.length - 1; i >= -1 && !resolvedAbsolute; i--) {
        var path = (i >= 0) ? args[i] : FS.cwd();
        // Skip empty and invalid entries
        if (typeof path != 'string') {
          throw new TypeError('Arguments to path.resolve must be strings');
        } else if (!path) {
          return ''; // an invalid portion invalidates the whole thing
        }
        resolvedPath = path + '/' + resolvedPath;
        resolvedAbsolute = PATH.isAbs(path);
      }
      // At this point the path should be resolved to a full absolute path, but
      // handle relative paths to be safe (might happen when process.cwd() fails)
      resolvedPath = PATH.normalizeArray(resolvedPath.split('/').filter((p) => !!p), !resolvedAbsolute).join('/');
      return ((resolvedAbsolute ? '/' : '') + resolvedPath) || '.';
    },
relative:(from, to) => {
      from = PATH_FS.resolve(from).slice(1);
      to = PATH_FS.resolve(to).slice(1);
      function trim(arr) {
        var start = 0;
        for (; start < arr.length; start++) {
          if (arr[start] !== '') break;
        }
        var end = arr.length - 1;
        for (; end >= 0; end--) {
          if (arr[end] !== '') break;
        }
        if (start > end) return [];
        return arr.slice(start, end - start + 1);
      }
      var fromParts = trim(from.split('/'));
      var toParts = trim(to.split('/'));
      var length = Math.min(fromParts.length, toParts.length);
      var samePartsLength = length;
      for (var i = 0; i < length; i++) {
        if (fromParts[i] !== toParts[i]) {
          samePartsLength = i;
          break;
        }
      }
      var outputParts = [];
      for (var i = samePartsLength; i < fromParts.length; i++) {
        outputParts.push('..');
      }
      outputParts = outputParts.concat(toParts.slice(samePartsLength));
      return outputParts.join('/');
    },
};


var UTF8Decoder = globalThis.TextDecoder && new TextDecoder();

var findStringEnd = (heapOrArray, idx, maxBytesToRead, ignoreNul) => {
    var maxIdx = idx + maxBytesToRead;
    if (ignoreNul) return maxIdx;
    // TextDecoder needs to know the byte length in advance, it doesn't stop on
    // null terminator by itself.
    // As a tiny code save trick, compare idx against maxIdx using a negation,
    // so that maxBytesToRead=undefined/NaN means Infinity.
    while (heapOrArray[idx] && !(idx >= maxIdx)) ++idx;
    return idx;
  };

  /**
   * Given a pointer 'idx' to a null-terminated UTF8-encoded string in the given
   * array that contains uint8 values, returns a copy of that string as a
   * Javascript String object.
   * heapOrArray is either a regular array, or a JavaScript typed array view.
   * @param {number=} idx
   * @param {number=} maxBytesToRead
   * @param {boolean=} ignoreNul - If true, the function will not stop on a NUL character.
   * @return {string}
   */
  var UTF8ArrayToString = (heapOrArray, idx = 0, maxBytesToRead, ignoreNul) => {
  
      var endPtr = findStringEnd(heapOrArray, idx, maxBytesToRead, ignoreNul);
  
      // When using conditional TextDecoder, skip it for short strings as the overhead of the native call is not worth it.
      if (endPtr - idx > 16 && heapOrArray.buffer && UTF8Decoder) {
        return UTF8Decoder.decode(heapOrArray.subarray(idx, endPtr));
      }
      var str = '';
      while (idx < endPtr) {
        // For UTF8 byte structure, see:
        // http://en.wikipedia.org/wiki/UTF-8#Description
        // https://www.ietf.org/rfc/rfc2279.txt
        // https://tools.ietf.org/html/rfc3629
        var u0 = heapOrArray[idx++];
        if (!(u0 & 0x80)) { str += String.fromCharCode(u0); continue; }
        var u1 = heapOrArray[idx++] & 63;
        if ((u0 & 0xE0) == 0xC0) { str += String.fromCharCode(((u0 & 31) << 6) | u1); continue; }
        var u2 = heapOrArray[idx++] & 63;
        if ((u0 & 0xF0) == 0xE0) {
          u0 = ((u0 & 15) << 12) | (u1 << 6) | u2;
        } else {
          u0 = ((u0 & 7) << 18) | (u1 << 12) | (u2 << 6) | (heapOrArray[idx++] & 63);
        }
  
        if (u0 < 0x10000) {
          str += String.fromCharCode(u0);
        } else {
          var ch = u0 - 0x10000;
          str += String.fromCharCode(0xD800 | (ch >> 10), 0xDC00 | (ch & 0x3FF));
        }
      }
      return str;
    };
  
  var FS_stdin_getChar_buffer = [];
  
  var lengthBytesUTF8 = (str) => {
      var len = 0;
      for (var i = 0; i < str.length; ++i) {
        // Gotcha: charCodeAt returns a 16-bit word that is a UTF-16 encoded code
        // unit, not a Unicode code point of the character! So decode
        // UTF16->UTF32->UTF8.
        // See http://unicode.org/faq/utf_bom.html#utf16-3
        var c = str.charCodeAt(i); // possibly a lead surrogate
        if (c <= 0x7F) {
          len++;
        } else if (c <= 0x7FF) {
          len += 2;
        } else if (c >= 0xD800 && c <= 0xDFFF) {
          len += 4; ++i;
        } else {
          len += 3;
        }
      }
      return len;
    };
  
  var stringToUTF8Array = (str, heap, outIdx, maxBytesToWrite) => {
      // Parameter maxBytesToWrite is not optional. Negative values, 0, null,
      // undefined and false each don't write out any bytes.
      if (!(maxBytesToWrite > 0))
        return 0;
  
      var startIdx = outIdx;
      var endIdx = outIdx + maxBytesToWrite - 1; // -1 for string null terminator.
      for (var i = 0; i < str.length; ++i) {
        // For UTF8 byte structure, see http://en.wikipedia.org/wiki/UTF-8#Description
        // and https://www.ietf.org/rfc/rfc2279.txt
        // and https://tools.ietf.org/html/rfc3629
        var u = str.codePointAt(i);
        if (u <= 0x7F) {
          if (outIdx >= endIdx) break;
          heap[outIdx++] = u;
        } else if (u <= 0x7FF) {
          if (outIdx + 1 >= endIdx) break;
          heap[outIdx++] = 0xC0 | (u >> 6);
          heap[outIdx++] = 0x80 | (u & 63);
        } else if (u <= 0xFFFF) {
          if (outIdx + 2 >= endIdx) break;
          heap[outIdx++] = 0xE0 | (u >> 12);
          heap[outIdx++] = 0x80 | ((u >> 6) & 63);
          heap[outIdx++] = 0x80 | (u & 63);
        } else {
          if (outIdx + 3 >= endIdx) break;
          heap[outIdx++] = 0xF0 | (u >> 18);
          heap[outIdx++] = 0x80 | ((u >> 12) & 63);
          heap[outIdx++] = 0x80 | ((u >> 6) & 63);
          heap[outIdx++] = 0x80 | (u & 63);
          // Gotcha: if codePoint is over 0xFFFF, it is represented as a surrogate pair in UTF-16.
          // We need to manually skip over the second code unit for correct iteration.
          i++;
        }
      }
      // Null-terminate the pointer to the buffer.
      heap[outIdx] = 0;
      return outIdx - startIdx;
    };
  /** @type {function(string, boolean=, number=)} */
  var intArrayFromString = (stringy, dontAddNull, length) => {
      var len = length > 0 ? length : lengthBytesUTF8(stringy)+1;
      var u8array = new Array(len);
      var numBytesWritten = stringToUTF8Array(stringy, u8array, 0, u8array.length);
      if (dontAddNull) u8array.length = numBytesWritten;
      return u8array;
    };
  var FS_stdin_getChar = () => {
      if (!FS_stdin_getChar_buffer.length) {
        var result = null;
        if (ENVIRONMENT_IS_NODE) {
          // we will read data by chunks of BUFSIZE
          var BUFSIZE = 256;
          var buf = Buffer.alloc(BUFSIZE);
          var bytesRead = 0;
  
          // For some reason we must suppress a closure warning here, even though
          // fd definitely exists on process.stdin, and is even the proper way to
          // get the fd of stdin,
          // https://github.com/nodejs/help/issues/2136#issuecomment-523649904
          // This started to happen after moving this logic out of library_tty.js,
          // so it is related to the surrounding code in some unclear manner.
          /** @suppress {missingProperties} */
          var fd = process.stdin.fd;
  
          try {
            bytesRead = fs.readSync(fd, buf, 0, BUFSIZE);
          } catch(e) {
            // Cross-platform differences: on Windows, reading EOF throws an
            // exception, but on other OSes, reading EOF returns 0. Uniformize
            // behavior by treating the EOF exception to return 0.
            if (e.toString().includes('EOF')) bytesRead = 0;
            else throw e;
          }
  
          if (bytesRead > 0) {
            result = buf.slice(0, bytesRead).toString('utf-8');
          }
        } else
        if (globalThis.window?.prompt) {
          // Browser.
          result = window.prompt('Input: ');  // returns null on cancel
          if (result !== null) {
            result += '\n';
          }
        } else
        {}
        if (!result) {
          return null;
        }
        FS_stdin_getChar_buffer = intArrayFromString(result, true);
      }
      return FS_stdin_getChar_buffer.shift();
    };
  var TTY = {
  ttys:[],
  init() {
        // https://github.com/emscripten-core/emscripten/pull/1555
        // if (ENVIRONMENT_IS_NODE) {
        //   // currently, FS.init does not distinguish if process.stdin is a file or TTY
        //   // device, it always assumes it's a TTY device. because of this, we're forcing
        //   // process.stdin to UTF8 encoding to at least make stdin reading compatible
        //   // with text files until FS.init can be refactored.
        //   process.stdin.setEncoding('utf8');
        // }
      },
  shutdown() {
        // https://github.com/emscripten-core/emscripten/pull/1555
        // if (ENVIRONMENT_IS_NODE) {
        //   // inolen: any idea as to why node -e 'process.stdin.read()' wouldn't exit immediately (with process.stdin being a tty)?
        //   // isaacs: because now it's reading from the stream, you've expressed interest in it, so that read() kicks off a _read() which creates a ReadReq operation
        //   // inolen: I thought read() in that case was a synchronous operation that just grabbed some amount of buffered data if it exists?
        //   // isaacs: it is. but it also triggers a _read() call, which calls readStart() on the handle
        //   // isaacs: do process.stdin.pause() and i'd think it'd probably close the pending call
        //   process.stdin.pause();
        // }
      },
  register(dev, ops) {
        TTY.ttys[dev] = { input: [], output: [], ops: ops };
        FS.registerDevice(dev, TTY.stream_ops);
      },
  stream_ops:{
  open(stream) {
          var tty = TTY.ttys[stream.node.rdev];
          if (!tty) {
            throw new FS.ErrnoError(43);
          }
          stream.tty = tty;
          stream.seekable = false;
        },
  close(stream) {
          // flush any pending line data
          stream.tty.ops.fsync(stream.tty);
        },
  fsync(stream) {
          stream.tty.ops.fsync(stream.tty);
        },
  read(stream, buffer, offset, length, pos /* ignored */) {
          if (!stream.tty || !stream.tty.ops.get_char) {
            throw new FS.ErrnoError(60);
          }
          var bytesRead = 0;
          for (var i = 0; i < length; i++) {
            var result;
            try {
              result = stream.tty.ops.get_char(stream.tty);
            } catch (e) {
              throw new FS.ErrnoError(29);
            }
            if (result === undefined && bytesRead === 0) {
              throw new FS.ErrnoError(6);
            }
            if (result === null || result === undefined) break;
            bytesRead++;
            buffer[offset+i] = result;
          }
          if (bytesRead) {
            stream.node.atime = Date.now();
          }
          return bytesRead;
        },
  write(stream, buffer, offset, length, pos) {
          if (!stream.tty || !stream.tty.ops.put_char) {
            throw new FS.ErrnoError(60);
          }
          try {
            for (var i = 0; i < length; i++) {
              stream.tty.ops.put_char(stream.tty, buffer[offset+i]);
            }
          } catch (e) {
            throw new FS.ErrnoError(29);
          }
          if (length) {
            stream.node.mtime = stream.node.ctime = Date.now();
          }
          return i;
        },
  },
  default_tty_ops:{
  get_char(tty) {
          return FS_stdin_getChar();
        },
  put_char(tty, val) {
          if (val === null || val === 10) {
            out(UTF8ArrayToString(tty.output));
            tty.output = [];
          } else {
            if (val != 0) tty.output.push(val); // val == 0 would cut text output off in the middle.
          }
        },
  fsync(tty) {
          if (tty.output?.length > 0) {
            out(UTF8ArrayToString(tty.output));
            tty.output = [];
          }
        },
  ioctl_tcgets(tty) {
          // typical setting
          return {
            c_iflag: 25856,
            c_oflag: 5,
            c_cflag: 191,
            c_lflag: 35387,
            c_cc: [
              0x03, 0x1c, 0x7f, 0x15, 0x04, 0x00, 0x01, 0x00, 0x11, 0x13, 0x1a, 0x00,
              0x12, 0x0f, 0x17, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            ]
          };
        },
  ioctl_tcsets(tty, optional_actions, data) {
          // currently just ignore
          return 0;
        },
  ioctl_tiocgwinsz(tty) {
          return [24, 80];
        },
  },
  default_tty1_ops:{
  put_char(tty, val) {
          if (val === null || val === 10) {
            err(UTF8ArrayToString(tty.output));
            tty.output = [];
          } else {
            if (val != 0) tty.output.push(val);
          }
        },
  fsync(tty) {
          if (tty.output?.length > 0) {
            err(UTF8ArrayToString(tty.output));
            tty.output = [];
          }
        },
  },
  };
  
  
  var zeroMemory = (ptr, size) => HEAPU8.fill(0, ptr, ptr + size);
  
  var alignMemory = (size, alignment) => {
      return Math.ceil(size / alignment) * alignment;
    };
  var mmapAlloc = (size) => {
      size = alignMemory(size, 65536);
      var ptr = _emscripten_builtin_memalign(65536, size);
      if (ptr) zeroMemory(ptr, size);
      return ptr;
    };
  var MEMFS = {
  ops_table:null,
  mount(mount) {
        return MEMFS.createNode(null, '/', 16895, 0);
      },
  createNode(parent, name, mode, dev) {
        if (FS.isBlkdev(mode) || FS.isFIFO(mode)) {
          // not supported
          throw new FS.ErrnoError(63);
        }
        MEMFS.ops_table ||= {
          dir: {
            node: {
              getattr: MEMFS.node_ops.getattr,
              setattr: MEMFS.node_ops.setattr,
              lookup: MEMFS.node_ops.lookup,
              mknod: MEMFS.node_ops.mknod,
              rename: MEMFS.node_ops.rename,
              unlink: MEMFS.node_ops.unlink,
              rmdir: MEMFS.node_ops.rmdir,
              readdir: MEMFS.node_ops.readdir,
              symlink: MEMFS.node_ops.symlink
            },
            stream: {
              llseek: MEMFS.stream_ops.llseek
            }
          },
          file: {
            node: {
              getattr: MEMFS.node_ops.getattr,
              setattr: MEMFS.node_ops.setattr
            },
            stream: {
              llseek: MEMFS.stream_ops.llseek,
              read: MEMFS.stream_ops.read,
              write: MEMFS.stream_ops.write,
              mmap: MEMFS.stream_ops.mmap,
              msync: MEMFS.stream_ops.msync
            }
          },
          link: {
            node: {
              getattr: MEMFS.node_ops.getattr,
              setattr: MEMFS.node_ops.setattr,
              readlink: MEMFS.node_ops.readlink
            },
            stream: {}
          },
          chrdev: {
            node: {
              getattr: MEMFS.node_ops.getattr,
              setattr: MEMFS.node_ops.setattr
            },
            stream: FS.chrdev_stream_ops
          }
        };
        var node = FS.createNode(parent, name, mode, dev);
        if (FS.isDir(node.mode)) {
          node.node_ops = MEMFS.ops_table.dir.node;
          node.stream_ops = MEMFS.ops_table.dir.stream;
          node.contents = {};
        } else if (FS.isFile(node.mode)) {
          node.node_ops = MEMFS.ops_table.file.node;
          node.stream_ops = MEMFS.ops_table.file.stream;
          // The actual number of bytes used in the typed array, as opposed to
          // contents.length which gives the whole capacity.
          node.usedBytes = 0;
          // The byte data of the file is stored in a typed array.
          // Note: typed arrays are not resizable like normal JS arrays are, so
          // there is a small penalty involved for appending file writes that
          // continuously grow a file similar to std::vector capacity vs used.
          node.contents = MEMFS.emptyFileContents ??= new Uint8Array(0);
        } else if (FS.isLink(node.mode)) {
          node.node_ops = MEMFS.ops_table.link.node;
          node.stream_ops = MEMFS.ops_table.link.stream;
        } else if (FS.isChrdev(node.mode)) {
          node.node_ops = MEMFS.ops_table.chrdev.node;
          node.stream_ops = MEMFS.ops_table.chrdev.stream;
        }
        node.atime = node.mtime = node.ctime = Date.now();
        // add the new node to the parent
        if (parent) {
          parent.contents[name] = node;
          parent.atime = parent.mtime = parent.ctime = node.atime;
        }
        return node;
      },
  getFileDataAsTypedArray(node) {
        return node.contents.subarray(0, node.usedBytes); // Make sure to not return excess unused bytes.
      },
  expandFileStorage(node, newCapacity) {
        var prevCapacity = node.contents.length;
        if (prevCapacity >= newCapacity) return; // No need to expand, the storage was already large enough.
        // Don't expand strictly to the given requested limit if it's only a very
        // small increase, but instead geometrically grow capacity.
        // For small filesizes (<1MB), perform size*2 geometric increase, but for
        // large sizes, do a much more conservative size*1.125 increase to avoid
        // overshooting the allocation cap by a very large margin.
        var CAPACITY_DOUBLING_MAX = 1024 * 1024;
        newCapacity = Math.max(newCapacity, (prevCapacity * (prevCapacity < CAPACITY_DOUBLING_MAX ? 2.0 : 1.125)) >>> 0);
        if (prevCapacity) newCapacity = Math.max(newCapacity, 256); // At minimum allocate 256b for each file when expanding.
        var oldContents = MEMFS.getFileDataAsTypedArray(node);
        node.contents = new Uint8Array(newCapacity); // Allocate new storage.
        node.contents.set(oldContents);
      },
  resizeFileStorage(node, newSize) {
        if (node.usedBytes == newSize) return;
        var oldContents = node.contents;
        node.contents = new Uint8Array(newSize); // Allocate new storage.
        node.contents.set(oldContents.subarray(0, Math.min(newSize, node.usedBytes))); // Copy old data over to the new storage.
        node.usedBytes = newSize;
      },
  node_ops:{
  getattr(node) {
          var attr = {};
          // device numbers reuse inode numbers.
          attr.dev = FS.isChrdev(node.mode) ? node.id : 1;
          attr.ino = node.id;
          attr.mode = node.mode;
          attr.nlink = 1;
          attr.uid = 0;
          attr.gid = 0;
          attr.rdev = node.rdev;
          if (FS.isDir(node.mode)) {
            attr.size = 4096;
          } else if (FS.isFile(node.mode)) {
            attr.size = node.usedBytes;
          } else if (FS.isLink(node.mode)) {
            attr.size = node.link.length;
          } else {
            attr.size = 0;
          }
          attr.atime = new Date(node.atime);
          attr.mtime = new Date(node.mtime);
          attr.ctime = new Date(node.ctime);
          // NOTE: In our implementation, st_blocks = Math.ceil(st_size/st_blksize),
          //       but this is not required by the standard.
          attr.blksize = 4096;
          attr.blocks = Math.ceil(attr.size / attr.blksize);
          return attr;
        },
  setattr(node, attr) {
          for (const key of ["mode", "atime", "mtime", "ctime"]) {
            if (attr[key] != null) {
              node[key] = attr[key];
            }
          }
          if (attr.size !== undefined) {
            MEMFS.resizeFileStorage(node, attr.size);
          }
        },
  lookup(parent, name) {
          // This error may happen quite a bit. To avoid overhead we reuse it (and
          // suffer a lack of stack info).
          if (!MEMFS.doesNotExistError) {
            MEMFS.doesNotExistError = new FS.ErrnoError(44);
            /** @suppress {checkTypes} */
            MEMFS.doesNotExistError.stack = '<generic error, no stack>';
          }
          throw MEMFS.doesNotExistError;
        },
  mknod(parent, name, mode, dev) {
          return MEMFS.createNode(parent, name, mode, dev);
        },
  rename(old_node, new_dir, new_name) {
          var new_node;
          try {
            new_node = FS.lookupNode(new_dir, new_name);
          } catch (e) {}
          if (new_node) {
            if (FS.isDir(old_node.mode)) {
              // if we're overwriting a directory at new_name, make sure it's empty.
              for (var i in new_node.contents) {
                throw new FS.ErrnoError(55);
              }
            }
            FS.hashRemoveNode(new_node);
          }
          // do the internal rewiring
          delete old_node.parent.contents[old_node.name];
          new_dir.contents[new_name] = old_node;
          old_node.name = new_name;
          new_dir.ctime = new_dir.mtime = old_node.parent.ctime = old_node.parent.mtime = Date.now();
        },
  unlink(parent, name) {
          delete parent.contents[name];
          parent.ctime = parent.mtime = Date.now();
        },
  rmdir(parent, name) {
          var node = FS.lookupNode(parent, name);
          for (var i in node.contents) {
            throw new FS.ErrnoError(55);
          }
          delete parent.contents[name];
          parent.ctime = parent.mtime = Date.now();
        },
  readdir(node) {
          return ['.', '..', ...Object.keys(node.contents)];
        },
  symlink(parent, newname, oldpath) {
          var node = MEMFS.createNode(parent, newname, 0o777 | 40960, 0);
          node.link = oldpath;
          return node;
        },
  readlink(node) {
          if (!FS.isLink(node.mode)) {
            throw new FS.ErrnoError(28);
          }
          return node.link;
        },
  },
  stream_ops:{
  read(stream, buffer, offset, length, position) {
          var contents = stream.node.contents;
          if (position >= stream.node.usedBytes) return 0;
          var size = Math.min(stream.node.usedBytes - position, length);
          buffer.set(contents.subarray(position, position + size), offset);
          return size;
        },
  write(stream, buffer, offset, length, position, canOwn) {
          // If the buffer is located in main memory (HEAP), and if
          // memory can grow, we can't hold on to references of the
          // memory buffer, as they may get invalidated. That means we
          // need to copy its contents.
          if (buffer.buffer === HEAP8.buffer) {
            canOwn = false;
          }
  
          if (!length) return 0;
          var node = stream.node;
          node.mtime = node.ctime = Date.now();
  
          if (canOwn) {
            node.contents = buffer.subarray(offset, offset + length);
            node.usedBytes = length;
          } else if (node.usedBytes === 0 && position === 0) { // If this is a simple first write to an empty file, do a fast set since we don't need to care about old data.
            node.contents = buffer.slice(offset, offset + length);
            node.usedBytes = length;
          } else {
            MEMFS.expandFileStorage(node, position+length);
            // Use typed array write which is available.
            node.contents.set(buffer.subarray(offset, offset + length), position);
            node.usedBytes = Math.max(node.usedBytes, position + length);
          }
          return length;
        },
  llseek(stream, offset, whence) {
          var position = offset;
          if (whence === 1) {
            position += stream.position;
          } else if (whence === 2) {
            if (FS.isFile(stream.node.mode)) {
              position += stream.node.usedBytes;
            }
          }
          if (position < 0) {
            throw new FS.ErrnoError(28);
          }
          return position;
        },
  mmap(stream, length, position, prot, flags) {
          if (!FS.isFile(stream.node.mode)) {
            throw new FS.ErrnoError(43);
          }
          var ptr;
          var allocated;
          var contents = stream.node.contents;
          // Only make a new copy when MAP_PRIVATE is specified.
          if (!(flags & 2) && contents.buffer === HEAP8.buffer) {
            // We can't emulate MAP_SHARED when the file is not backed by the
            // buffer we're mapping to (e.g. the HEAP buffer).
            allocated = false;
            ptr = contents.byteOffset;
          } else {
            allocated = true;
            ptr = mmapAlloc(length);
            if (!ptr) {
              throw new FS.ErrnoError(48);
            }
            if (contents) {
              // Try to avoid unnecessary slices.
              if (position > 0 || position + length < contents.length) {
                if (contents.subarray) {
                  contents = contents.subarray(position, position + length);
                } else {
                  contents = Array.prototype.slice.call(contents, position, position + length);
                }
              }
              HEAP8.set(contents, ptr);
            }
          }
          return { ptr, allocated };
        },
  msync(stream, buffer, offset, length, mmapFlags) {
          MEMFS.stream_ops.write(stream, buffer, 0, length, offset, false);
          // should we check if bytesWritten and length are the same?
          return 0;
        },
  },
  };
  
  var FS_modeStringToFlags = (str) => {
      if (typeof str != 'string') return str;
      var flagModes = {
        'r': 0,
        'r+': 2,
        'w': 512 | 64 | 1,
        'w+': 512 | 64 | 2,
        'a': 1024 | 64 | 1,
        'a+': 1024 | 64 | 2,
      };
      var flags = flagModes[str];
      if (typeof flags == 'undefined') {
        throw new Error(`Unknown file open mode: ${str}`);
      }
      return flags;
    };
  
  var FS_fileDataToTypedArray = (data) => {
      if (typeof data == 'string') {
        data = intArrayFromString(data, true);
      }
      if (!data.subarray) {
        data = new Uint8Array(data);
      }
      return data;
    };
  
  var FS_getMode = (canRead, canWrite) => {
      var mode = 0;
      if (canRead) mode |= 292 | 73;
      if (canWrite) mode |= 146;
      return mode;
    };
  
  
  var asyncLoad = async (url) => {
      var arrayBuffer = await readAsync(url);
      return new Uint8Array(arrayBuffer);
    };
  
  
  var FS_createDataFile = (...args) => FS.createDataFile(...args);
  
  var getUniqueRunDependency = (id) => {
      return id;
    };
  
  
  
  var preloadPlugins = [];
  var FS_handledByPreloadPlugin = async (byteArray, fullname) => {
      // Ensure plugins are ready.
      if (typeof Browser != 'undefined') Browser.init();
  
      for (var plugin of preloadPlugins) {
        if (plugin['canHandle'](fullname)) {
          return plugin['handle'](byteArray, fullname);
        }
      }
      // If no plugin handled this file then return the original/unmodified
      // byteArray.
      return byteArray;
    };
  var FS_preloadFile = async (parent, name, url, canRead, canWrite, dontCreateFile, canOwn, preFinish) => {
      // TODO we should allow people to just pass in a complete filename instead
      // of parent and name being that we just join them anyways
      var fullname = name ? PATH_FS.resolve(PATH.join2(parent, name)) : parent;
      var dep = getUniqueRunDependency(`cp ${fullname}`); // might have several active requests for the same fullname
      addRunDependency(dep);
  
      try {
        var byteArray = url;
        if (typeof url == 'string') {
          byteArray = await asyncLoad(url);
        }
  
        byteArray = await FS_handledByPreloadPlugin(byteArray, fullname);
        preFinish?.();
        if (!dontCreateFile) {
          FS_createDataFile(parent, name, byteArray, canRead, canWrite, canOwn);
        }
      } finally {
        removeRunDependency(dep);
      }
    };
  var FS_createPreloadedFile = (parent, name, url, canRead, canWrite, onload, onerror, dontCreateFile, canOwn, preFinish) => {
      FS_preloadFile(parent, name, url, canRead, canWrite, dontCreateFile, canOwn, preFinish).then(onload).catch(onerror);
    };
  var FS = {
  root:null,
  mounts:[],
  devices:{
  },
  streams:[],
  nextInode:1,
  nameTable:null,
  currentPath:"/",
  initialized:false,
  ignorePermissions:true,
  filesystems:null,
  syncFSRequests:0,
  ErrnoError:class {
        name = 'ErrnoError';
        // We set the `name` property to be able to identify `FS.ErrnoError`
        // - the `name` is a standard ECMA-262 property of error objects. Kind of good to have it anyway.
        // - when using PROXYFS, an error can come from an underlying FS
        // as different FS objects have their own FS.ErrnoError each,
        // the test `err instanceof FS.ErrnoError` won't detect an error coming from another filesystem, causing bugs.
        // we'll use the reliable test `err.name == "ErrnoError"` instead
        constructor(errno) {
          this.errno = errno;
        }
      },
  FSStream:class {
        shared = {};
        get object() {
          return this.node;
        }
        set object(val) {
          this.node = val;
        }
        get isRead() {
          return (this.flags & 2097155) !== 1;
        }
        get isWrite() {
          return (this.flags & 2097155) !== 0;
        }
        get isAppend() {
          return (this.flags & 1024);
        }
        get flags() {
          return this.shared.flags;
        }
        set flags(val) {
          this.shared.flags = val;
        }
        get position() {
          return this.shared.position;
        }
        set position(val) {
          this.shared.position = val;
        }
      },
  FSNode:class {
        node_ops = {};
        stream_ops = {};
        readMode = 292 | 73;
        writeMode = 146;
        mounted = null;
        constructor(parent, name, mode, rdev) {
          if (!parent) {
            parent = this;  // root node sets parent to itself
          }
          this.parent = parent;
          this.mount = parent.mount;
          this.id = FS.nextInode++;
          this.name = name;
          this.mode = mode;
          this.rdev = rdev;
          this.atime = this.mtime = this.ctime = Date.now();
        }
        get read() {
          return (this.mode & this.readMode) === this.readMode;
        }
        set read(val) {
          val ? this.mode |= this.readMode : this.mode &= ~this.readMode;
        }
        get write() {
          return (this.mode & this.writeMode) === this.writeMode;
        }
        set write(val) {
          val ? this.mode |= this.writeMode : this.mode &= ~this.writeMode;
        }
        get isFolder() {
          return FS.isDir(this.mode);
        }
        get isDevice() {
          return FS.isChrdev(this.mode);
        }
      },
  lookupPath(path, opts = {}) {
        if (!path) {
          throw new FS.ErrnoError(44);
        }
        opts.follow_mount ??= true
  
        if (!PATH.isAbs(path)) {
          path = FS.cwd() + '/' + path;
        }
  
        // limit max consecutive symlinks to SYMLOOP_MAX.
        linkloop: for (var nlinks = 0; nlinks < 40; nlinks++) {
          // split the absolute path
          var parts = path.split('/').filter((p) => !!p);
  
          // start at the root
          var current = FS.root;
          var current_path = '/';
  
          for (var i = 0; i < parts.length; i++) {
            var islast = (i === parts.length-1);
            if (islast && opts.parent) {
              // stop resolving
              break;
            }
  
            if (parts[i] === '.') {
              continue;
            }
  
            if (parts[i] === '..') {
              current_path = PATH.dirname(current_path);
              if (FS.isRoot(current)) {
                path = current_path + '/' + parts.slice(i + 1).join('/');
                // We're making progress here, don't let many consecutive ..'s
                // lead to ELOOP
                nlinks--;
                continue linkloop;
              } else {
                current = current.parent;
              }
              continue;
            }
  
            current_path = PATH.join2(current_path, parts[i]);
            try {
              current = FS.lookupNode(current, parts[i]);
            } catch (e) {
              // if noent_okay is true, suppress a ENOENT in the last component
              // and return an object with an undefined node. This is needed for
              // resolving symlinks in the path when creating a file.
              if ((e?.errno === 44) && islast && opts.noent_okay) {
                return { path: current_path };
              }
              throw e;
            }
  
            // jump to the mount's root node if this is a mountpoint
            if (FS.isMountpoint(current) && (!islast || opts.follow_mount)) {
              current = current.mounted.root;
            }
  
            // by default, lookupPath will not follow a symlink if it is the final path component.
            // setting opts.follow = true will override this behavior.
            if (FS.isLink(current.mode) && (!islast || opts.follow)) {
              if (!current.node_ops.readlink) {
                throw new FS.ErrnoError(52);
              }
              var link = current.node_ops.readlink(current);
              if (!PATH.isAbs(link)) {
                link = PATH.dirname(current_path) + '/' + link;
              }
              path = link + '/' + parts.slice(i + 1).join('/');
              continue linkloop;
            }
          }
          return { path: current_path, node: current };
        }
        throw new FS.ErrnoError(32);
      },
  getPath(node) {
        var path;
        while (true) {
          if (FS.isRoot(node)) {
            var mount = node.mount.mountpoint;
            if (!path) return mount;
            return mount[mount.length-1] !== '/' ? `${mount}/${path}` : mount + path;
          }
          path = path ? `${node.name}/${path}` : node.name;
          node = node.parent;
        }
      },
  hashName(parentid, name) {
        var hash = 0;
  
        for (var i = 0; i < name.length; i++) {
          hash = ((hash << 5) - hash + name.charCodeAt(i)) | 0;
        }
        return ((parentid + hash) >>> 0) % FS.nameTable.length;
      },
  hashAddNode(node) {
        var hash = FS.hashName(node.parent.id, node.name);
        node.name_next = FS.nameTable[hash];
        FS.nameTable[hash] = node;
      },
  hashRemoveNode(node) {
        var hash = FS.hashName(node.parent.id, node.name);
        if (FS.nameTable[hash] === node) {
          FS.nameTable[hash] = node.name_next;
        } else {
          var current = FS.nameTable[hash];
          while (current) {
            if (current.name_next === node) {
              current.name_next = node.name_next;
              break;
            }
            current = current.name_next;
          }
        }
      },
  lookupNode(parent, name) {
        var errCode = FS.mayLookup(parent);
        if (errCode) {
          throw new FS.ErrnoError(errCode);
        }
        var hash = FS.hashName(parent.id, name);
        for (var node = FS.nameTable[hash]; node; node = node.name_next) {
          var nodeName = node.name;
          if (node.parent.id === parent.id && nodeName === name) {
            return node;
          }
        }
        // if we failed to find it in the cache, call into the VFS
        return FS.lookup(parent, name);
      },
  createNode(parent, name, mode, rdev) {
        var node = new FS.FSNode(parent, name, mode, rdev);
  
        FS.hashAddNode(node);
  
        return node;
      },
  destroyNode(node) {
        FS.hashRemoveNode(node);
      },
  isRoot(node) {
        return node === node.parent;
      },
  isMountpoint(node) {
        return !!node.mounted;
      },
  isFile(mode) {
        return (mode & 61440) === 32768;
      },
  isDir(mode) {
        return (mode & 61440) === 16384;
      },
  isLink(mode) {
        return (mode & 61440) === 40960;
      },
  isChrdev(mode) {
        return (mode & 61440) === 8192;
      },
  isBlkdev(mode) {
        return (mode & 61440) === 24576;
      },
  isFIFO(mode) {
        return (mode & 61440) === 4096;
      },
  isSocket(mode) {
        return (mode & 49152) === 49152;
      },
  flagsToPermissionString(flag) {
        var perms = ['r', 'w', 'rw'][flag & 3];
        if ((flag & 512)) {
          perms += 'w';
        }
        return perms;
      },
  nodePermissions(node, perms) {
        if (FS.ignorePermissions) {
          return 0;
        }
        // return 0 if any user, group or owner bits are set.
        if (perms.includes('r') && !(node.mode & 292)) {
          return 2;
        }
        if (perms.includes('w') && !(node.mode & 146)) {
          return 2;
        }
        if (perms.includes('x') && !(node.mode & 73)) {
          return 2;
        }
        return 0;
      },
  mayLookup(dir) {
        if (!FS.isDir(dir.mode)) return 54;
        var errCode = FS.nodePermissions(dir, 'x');
        if (errCode) return errCode;
        if (!dir.node_ops.lookup) return 2;
        return 0;
      },
  mayCreate(dir, name) {
        if (!FS.isDir(dir.mode)) {
          return 54;
        }
        try {
          var node = FS.lookupNode(dir, name);
          return 20;
        } catch (e) {
        }
        return FS.nodePermissions(dir, 'wx');
      },
  mayDelete(dir, name, isdir) {
        var node;
        try {
          node = FS.lookupNode(dir, name);
        } catch (e) {
          return e.errno;
        }
        var errCode = FS.nodePermissions(dir, 'wx');
        if (errCode) {
          return errCode;
        }
        if (isdir) {
          if (!FS.isDir(node.mode)) {
            return 54;
          }
          if (FS.isRoot(node) || FS.getPath(node) === FS.cwd()) {
            return 10;
          }
        } else if (FS.isDir(node.mode)) {
          return 31;
        }
        return 0;
      },
  mayOpen(node, flags) {
        if (!node) {
          return 44;
        }
        if (FS.isLink(node.mode)) {
          return 32;
        }
        var mode = FS.flagsToPermissionString(flags);
        if (FS.isDir(node.mode)) {
          // opening for write
          // TODO: check for O_SEARCH? (== search for dir only)
          if (mode !== 'r' || (flags & (512 | 64))) {
            return 31;
          }
        }
        return FS.nodePermissions(node, mode);
      },
  checkOpExists(op, err) {
        if (!op) {
          throw new FS.ErrnoError(err);
        }
        return op;
      },
  MAX_OPEN_FDS:4096,
  nextfd() {
        for (var fd = 0; fd <= FS.MAX_OPEN_FDS; fd++) {
          if (!FS.streams[fd]) {
            return fd;
          }
        }
        throw new FS.ErrnoError(33);
      },
  getStreamChecked(fd) {
        var stream = FS.getStream(fd);
        if (!stream) {
          throw new FS.ErrnoError(8);
        }
        return stream;
      },
  getStream:(fd) => FS.streams[fd],
  createStream(stream, fd = -1) {
  
        // clone it, so we can return an instance of FSStream
        stream = Object.assign(new FS.FSStream(), stream);
        if (fd == -1) {
          fd = FS.nextfd();
        }
        stream.fd = fd;
        FS.streams[fd] = stream;
        return stream;
      },
  closeStream(fd) {
        FS.streams[fd] = null;
      },
  dupStream(origStream, fd = -1) {
        var stream = FS.createStream(origStream, fd);
        stream.stream_ops?.dup?.(stream);
        return stream;
      },
  doSetAttr(stream, node, attr) {
        var setattr = stream?.stream_ops.setattr;
        var arg = setattr ? stream : node;
        setattr ??= node.node_ops.setattr;
        FS.checkOpExists(setattr, 63)
        try {
          setattr(arg, attr);
        } catch (e) {
          if (e instanceof RangeError) {
            throw new FS.ErrnoError(22);
          }
          throw e;
        }
      },
  chrdev_stream_ops:{
  open(stream) {
          var device = FS.getDevice(stream.node.rdev);
          // override node's stream ops with the device's
          stream.stream_ops = device.stream_ops;
          // forward the open call
          stream.stream_ops.open?.(stream);
        },
  llseek() {
          throw new FS.ErrnoError(70);
        },
  },
  major:(dev) => ((dev) >> 8),
  minor:(dev) => ((dev) & 0xff),
  makedev:(ma, mi) => ((ma) << 8 | (mi)),
  registerDevice(dev, ops) {
        FS.devices[dev] = { stream_ops: ops };
      },
  getDevice:(dev) => FS.devices[dev],
  getMounts(mount) {
        var mounts = [];
        var check = [mount];
  
        while (check.length) {
          var m = check.pop();
  
          mounts.push(m);
  
          check.push(...m.mounts);
        }
  
        return mounts;
      },
  syncfs(populate, callback) {
        if (typeof populate == 'function') {
          callback = populate;
          populate = false;
        }
  
        FS.syncFSRequests++;
  
        if (FS.syncFSRequests > 1) {
          err(`warning: ${FS.syncFSRequests} FS.syncfs operations in flight at once, probably just doing extra work`);
        }
  
        var mounts = FS.getMounts(FS.root.mount);
        var completed = 0;
  
        function doCallback(errCode) {
          FS.syncFSRequests--;
          return callback(errCode);
        }
  
        function done(errCode) {
          if (errCode) {
            if (!done.errored) {
              done.errored = true;
              return doCallback(errCode);
            }
            return;
          }
          if (++completed >= mounts.length) {
            doCallback(null);
          }
        };
  
        // sync all mounts
        for (var mount of mounts) {
          if (mount.type.syncfs) {
            mount.type.syncfs(mount, populate, done);
          } else {
            done(null);
          }
        }
      },
  mount(type, opts, mountpoint) {
        var root = mountpoint === '/';
        var pseudo = !mountpoint;
        var node;
  
        if (root && FS.root) {
          throw new FS.ErrnoError(10);
        } else if (!root && !pseudo) {
          var lookup = FS.lookupPath(mountpoint, { follow_mount: false });
  
          mountpoint = lookup.path;  // use the absolute path
          node = lookup.node;
  
          if (FS.isMountpoint(node)) {
            throw new FS.ErrnoError(10);
          }
  
          if (!FS.isDir(node.mode)) {
            throw new FS.ErrnoError(54);
          }
        }
  
        var mount = {
          type,
          opts,
          mountpoint,
          mounts: []
        };
  
        // create a root node for the fs
        var mountRoot = type.mount(mount);
        mountRoot.mount = mount;
        mount.root = mountRoot;
  
        if (root) {
          FS.root = mountRoot;
        } else if (node) {
          // set as a mountpoint
          node.mounted = mount;
  
          // add the new mount to the current mount's children
          if (node.mount) {
            node.mount.mounts.push(mount);
          }
        }
  
        return mountRoot;
      },
  unmount(mountpoint) {
        var lookup = FS.lookupPath(mountpoint, { follow_mount: false });
  
        if (!FS.isMountpoint(lookup.node)) {
          throw new FS.ErrnoError(28);
        }
  
        // destroy the nodes for this mount, and all its child mounts
        var node = lookup.node;
        var mount = node.mounted;
        var mounts = FS.getMounts(mount);
  
        for (var [hash, current] of Object.entries(FS.nameTable)) {
          while (current) {
            var next = current.name_next;
  
            if (mounts.includes(current.mount)) {
              FS.destroyNode(current);
            }
  
            current = next;
          }
        }
  
        // no longer a mountpoint
        node.mounted = null;
  
        // remove this mount from the child mounts
        var idx = node.mount.mounts.indexOf(mount);
        node.mount.mounts.splice(idx, 1);
      },
  lookup(parent, name) {
        return parent.node_ops.lookup(parent, name);
      },
  mknod(path, mode, dev) {
        var lookup = FS.lookupPath(path, { parent: true });
        var parent = lookup.node;
        var name = PATH.basename(path);
        if (!name) {
          throw new FS.ErrnoError(28);
        }
        if (name === '.' || name === '..') {
          throw new FS.ErrnoError(20);
        }
        var errCode = FS.mayCreate(parent, name);
        if (errCode) {
          throw new FS.ErrnoError(errCode);
        }
        if (!parent.node_ops.mknod) {
          throw new FS.ErrnoError(63);
        }
        return parent.node_ops.mknod(parent, name, mode, dev);
      },
  statfs(path) {
        return FS.statfsNode(FS.lookupPath(path, {follow: true}).node);
      },
  statfsStream(stream) {
        // We keep a separate statfsStream function because noderawfs overrides
        // it. In noderawfs, stream.node is sometimes null. Instead, we need to
        // look at stream.path.
        return FS.statfsNode(stream.node);
      },
  statfsNode(node) {
        // NOTE: None of the defaults here are true. We're just returning safe and
        //       sane values. Currently nodefs and rawfs replace these defaults,
        //       other file systems leave them alone.
        var rtn = {
          bsize: 4096,
          frsize: 4096,
          blocks: 1e6,
          bfree: 5e5,
          bavail: 5e5,
          files: FS.nextInode,
          ffree: FS.nextInode - 1,
          fsid: 42,
          flags: 2,
          namelen: 255,
        };
  
        if (node.node_ops.statfs) {
          Object.assign(rtn, node.node_ops.statfs(node.mount.opts.root));
        }
        return rtn;
      },
  create(path, mode = 0o666) {
        mode &= 4095;
        mode |= 32768;
        return FS.mknod(path, mode, 0);
      },
  mkdir(path, mode = 0o777) {
        mode &= 511 | 512;
        mode |= 16384;
        return FS.mknod(path, mode, 0);
      },
  mkdirTree(path, mode) {
        var dirs = path.split('/');
        var d = '';
        for (var dir of dirs) {
          if (!dir) continue;
          if (d || PATH.isAbs(path)) d += '/';
          d += dir;
          try {
            FS.mkdir(d, mode);
          } catch(e) {
            if (e.errno != 20) throw e;
          }
        }
      },
  mkdev(path, mode, dev) {
        if (typeof dev == 'undefined') {
          dev = mode;
          mode = 0o666;
        }
        mode |= 8192;
        return FS.mknod(path, mode, dev);
      },
  symlink(oldpath, newpath) {
        if (!PATH_FS.resolve(oldpath)) {
          throw new FS.ErrnoError(44);
        }
        var lookup = FS.lookupPath(newpath, { parent: true });
        var parent = lookup.node;
        if (!parent) {
          throw new FS.ErrnoError(44);
        }
        var newname = PATH.basename(newpath);
        var errCode = FS.mayCreate(parent, newname);
        if (errCode) {
          throw new FS.ErrnoError(errCode);
        }
        if (!parent.node_ops.symlink) {
          throw new FS.ErrnoError(63);
        }
        return parent.node_ops.symlink(parent, newname, oldpath);
      },
  rename(old_path, new_path) {
        var old_dirname = PATH.dirname(old_path);
        var new_dirname = PATH.dirname(new_path);
        var old_name = PATH.basename(old_path);
        var new_name = PATH.basename(new_path);
        // parents must exist
        var lookup, old_dir, new_dir;
  
        // let the errors from non existent directories percolate up
        lookup = FS.lookupPath(old_path, { parent: true });
        old_dir = lookup.node;
        lookup = FS.lookupPath(new_path, { parent: true });
        new_dir = lookup.node;
  
        if (!old_dir || !new_dir) throw new FS.ErrnoError(44);
        // need to be part of the same mount
        if (old_dir.mount !== new_dir.mount) {
          throw new FS.ErrnoError(75);
        }
        // source must exist
        var old_node = FS.lookupNode(old_dir, old_name);
        // old path should not be an ancestor of the new path
        var relative = PATH_FS.relative(old_path, new_dirname);
        if (relative.charAt(0) !== '.') {
          throw new FS.ErrnoError(28);
        }
        // new path should not be an ancestor of the old path
        relative = PATH_FS.relative(new_path, old_dirname);
        if (relative.charAt(0) !== '.') {
          throw new FS.ErrnoError(55);
        }
        // see if the new path already exists
        var new_node;
        try {
          new_node = FS.lookupNode(new_dir, new_name);
        } catch (e) {
          // not fatal
        }
        // early out if nothing needs to change
        if (old_node === new_node) {
          return;
        }
        // we'll need to delete the old entry
        var isdir = FS.isDir(old_node.mode);
        var errCode = FS.mayDelete(old_dir, old_name, isdir);
        if (errCode) {
          throw new FS.ErrnoError(errCode);
        }
        // need delete permissions if we'll be overwriting.
        // need create permissions if new doesn't already exist.
        errCode = new_node ?
          FS.mayDelete(new_dir, new_name, isdir) :
          FS.mayCreate(new_dir, new_name);
        if (errCode) {
          throw new FS.ErrnoError(errCode);
        }
        if (!old_dir.node_ops.rename) {
          throw new FS.ErrnoError(63);
        }
        if (FS.isMountpoint(old_node) || (new_node && FS.isMountpoint(new_node))) {
          throw new FS.ErrnoError(10);
        }
        // if we are going to change the parent, check write permissions
        if (new_dir !== old_dir) {
          errCode = FS.nodePermissions(old_dir, 'w');
          if (errCode) {
            throw new FS.ErrnoError(errCode);
          }
        }
        // remove the node from the lookup hash
        FS.hashRemoveNode(old_node);
        // do the underlying fs rename
        try {
          old_dir.node_ops.rename(old_node, new_dir, new_name);
          // update old node (we do this here to avoid each backend
          // needing to)
          old_node.parent = new_dir;
        } catch (e) {
          throw e;
        } finally {
          // add the node back to the hash (in case node_ops.rename
          // changed its name)
          FS.hashAddNode(old_node);
        }
      },
  rmdir(path) {
        var lookup = FS.lookupPath(path, { parent: true });
        var parent = lookup.node;
        var name = PATH.basename(path);
        var node = FS.lookupNode(parent, name);
        var errCode = FS.mayDelete(parent, name, true);
        if (errCode) {
          throw new FS.ErrnoError(errCode);
        }
        if (!parent.node_ops.rmdir) {
          throw new FS.ErrnoError(63);
        }
        if (FS.isMountpoint(node)) {
          throw new FS.ErrnoError(10);
        }
        parent.node_ops.rmdir(parent, name);
        FS.destroyNode(node);
      },
  readdir(path) {
        var lookup = FS.lookupPath(path, { follow: true });
        var node = lookup.node;
        var readdir = FS.checkOpExists(node.node_ops.readdir, 54);
        return readdir(node);
      },
  unlink(path) {
        var lookup = FS.lookupPath(path, { parent: true });
        var parent = lookup.node;
        if (!parent) {
          throw new FS.ErrnoError(44);
        }
        var name = PATH.basename(path);
        var node = FS.lookupNode(parent, name);
        var errCode = FS.mayDelete(parent, name, false);
        if (errCode) {
          // According to POSIX, we should map EISDIR to EPERM, but
          // we instead do what Linux does (and we must, as we use
          // the musl linux libc).
          throw new FS.ErrnoError(errCode);
        }
        if (!parent.node_ops.unlink) {
          throw new FS.ErrnoError(63);
        }
        if (FS.isMountpoint(node)) {
          throw new FS.ErrnoError(10);
        }
        parent.node_ops.unlink(parent, name);
        FS.destroyNode(node);
      },
  readlink(path) {
        var lookup = FS.lookupPath(path);
        var link = lookup.node;
        if (!link) {
          throw new FS.ErrnoError(44);
        }
        if (!link.node_ops.readlink) {
          throw new FS.ErrnoError(28);
        }
        return link.node_ops.readlink(link);
      },
  stat(path, dontFollow) {
        var lookup = FS.lookupPath(path, { follow: !dontFollow });
        var node = lookup.node;
        var getattr = FS.checkOpExists(node.node_ops.getattr, 63);
        return getattr(node);
      },
  fstat(fd) {
        var stream = FS.getStreamChecked(fd);
        var node = stream.node;
        var getattr = stream.stream_ops.getattr;
        var arg = getattr ? stream : node;
        getattr ??= node.node_ops.getattr;
        FS.checkOpExists(getattr, 63)
        return getattr(arg);
      },
  lstat(path) {
        return FS.stat(path, true);
      },
  doChmod(stream, node, mode, dontFollow) {
        FS.doSetAttr(stream, node, {
          mode: (mode & 4095) | (node.mode & ~4095),
          ctime: Date.now(),
          dontFollow
        });
      },
  chmod(path, mode, dontFollow) {
        var node;
        if (typeof path == 'string') {
          var lookup = FS.lookupPath(path, { follow: !dontFollow });
          node = lookup.node;
        } else {
          node = path;
        }
        FS.doChmod(null, node, mode, dontFollow);
      },
  lchmod(path, mode) {
        FS.chmod(path, mode, true);
      },
  fchmod(fd, mode) {
        var stream = FS.getStreamChecked(fd);
        FS.doChmod(stream, stream.node, mode, false);
      },
  doChown(stream, node, dontFollow) {
        FS.doSetAttr(stream, node, {
          timestamp: Date.now(),
          dontFollow
          // we ignore the uid / gid for now
        });
      },
  chown(path, uid, gid, dontFollow) {
        var node;
        if (typeof path == 'string') {
          var lookup = FS.lookupPath(path, { follow: !dontFollow });
          node = lookup.node;
        } else {
          node = path;
        }
        FS.doChown(null, node, dontFollow);
      },
  lchown(path, uid, gid) {
        FS.chown(path, uid, gid, true);
      },
  fchown(fd, uid, gid) {
        var stream = FS.getStreamChecked(fd);
        FS.doChown(stream, stream.node, false);
      },
  doTruncate(stream, node, len) {
        if (FS.isDir(node.mode)) {
          throw new FS.ErrnoError(31);
        }
        if (!FS.isFile(node.mode)) {
          throw new FS.ErrnoError(28);
        }
        var errCode = FS.nodePermissions(node, 'w');
        if (errCode) {
          throw new FS.ErrnoError(errCode);
        }
        FS.doSetAttr(stream, node, {
          size: len,
          timestamp: Date.now()
        });
      },
  truncate(path, len) {
        if (len < 0) {
          throw new FS.ErrnoError(28);
        }
        var node;
        if (typeof path == 'string') {
          var lookup = FS.lookupPath(path, { follow: true });
          node = lookup.node;
        } else {
          node = path;
        }
        FS.doTruncate(null, node, len);
      },
  ftruncate(fd, len) {
        var stream = FS.getStreamChecked(fd);
        if (len < 0 || (stream.flags & 2097155) === 0) {
          throw new FS.ErrnoError(28);
        }
        FS.doTruncate(stream, stream.node, len);
      },
  utime(path, atime, mtime) {
        var lookup = FS.lookupPath(path, { follow: true });
        var node = lookup.node;
        var setattr = FS.checkOpExists(node.node_ops.setattr, 63);
        setattr(node, {
          atime: atime,
          mtime: mtime
        });
      },
  open(path, flags, mode = 0o666) {
        if (path === "") {
          throw new FS.ErrnoError(44);
        }
        flags = FS_modeStringToFlags(flags);
        if ((flags & 64)) {
          mode = (mode & 4095) | 32768;
        } else {
          mode = 0;
        }
        var node;
        var isDirPath;
        if (typeof path == 'object') {
          node = path;
        } else {
          isDirPath = path.endsWith("/");
          // noent_okay makes it so that if the final component of the path
          // doesn't exist, lookupPath returns `node: undefined`. `path` will be
          // updated to point to the target of all symlinks.
          var lookup = FS.lookupPath(path, {
            follow: !(flags & 131072),
            noent_okay: true
          });
          node = lookup.node;
          path = lookup.path;
        }
        // perhaps we need to create the node
        var created = false;
        if ((flags & 64)) {
          if (node) {
            // if O_CREAT and O_EXCL are set, error out if the node already exists
            if ((flags & 128)) {
              throw new FS.ErrnoError(20);
            }
          } else if (isDirPath) {
            throw new FS.ErrnoError(31);
          } else {
            // node doesn't exist, try to create it
            // Ignore the permission bits here to ensure we can `open` this new
            // file below. We use chmod below to apply the permissions once the
            // file is open.
            node = FS.mknod(path, mode | 0o777, 0);
            created = true;
          }
        }
        if (!node) {
          throw new FS.ErrnoError(44);
        }
        // can't truncate a device
        if (FS.isChrdev(node.mode)) {
          flags &= ~512;
        }
        // if asked only for a directory, then this must be one
        if ((flags & 65536) && !FS.isDir(node.mode)) {
          throw new FS.ErrnoError(54);
        }
        // check permissions, if this is not a file we just created now (it is ok to
        // create and write to a file with read-only permissions; it is read-only
        // for later use)
        if (!created) {
          var errCode = FS.mayOpen(node, flags);
          if (errCode) {
            throw new FS.ErrnoError(errCode);
          }
        }
        // do truncation if necessary
        if ((flags & 512) && !created) {
          FS.truncate(node, 0);
        }
        // we've already handled these, don't pass down to the underlying vfs
        flags &= ~(128 | 512 | 131072);
  
        // register the stream with the filesystem
        var stream = FS.createStream({
          node,
          path: FS.getPath(node),  // we want the absolute path to the node
          flags,
          seekable: true,
          position: 0,
          stream_ops: node.stream_ops,
          // used by the file family libc calls (fopen, fwrite, ferror, etc.)
          ungotten: [],
          error: false
        });
        // call the new stream's open function
        if (stream.stream_ops.open) {
          stream.stream_ops.open(stream);
        }
        if (created) {
          FS.chmod(node, mode & 0o777);
        }
        return stream;
      },
  close(stream) {
        if (FS.isClosed(stream)) {
          throw new FS.ErrnoError(8);
        }
        if (stream.getdents) stream.getdents = null; // free readdir state
        try {
          if (stream.stream_ops.close) {
            stream.stream_ops.close(stream);
          }
        } catch (e) {
          throw e;
        } finally {
          FS.closeStream(stream.fd);
        }
        stream.fd = null;
      },
  isClosed(stream) {
        return stream.fd === null;
      },
  llseek(stream, offset, whence) {
        if (FS.isClosed(stream)) {
          throw new FS.ErrnoError(8);
        }
        if (!stream.seekable || !stream.stream_ops.llseek) {
          throw new FS.ErrnoError(70);
        }
        if (whence != 0 && whence != 1 && whence != 2) {
          throw new FS.ErrnoError(28);
        }
        stream.position = stream.stream_ops.llseek(stream, offset, whence);
        stream.ungotten = [];
        return stream.position;
      },
  read(stream, buffer, offset, length, position) {
        if (length < 0 || position < 0) {
          throw new FS.ErrnoError(28);
        }
        if (FS.isClosed(stream)) {
          throw new FS.ErrnoError(8);
        }
        if ((stream.flags & 2097155) === 1) {
          throw new FS.ErrnoError(8);
        }
        if (FS.isDir(stream.node.mode)) {
          throw new FS.ErrnoError(31);
        }
        if (!stream.stream_ops.read) {
          throw new FS.ErrnoError(28);
        }
        var seeking = typeof position != 'undefined';
        if (!seeking) {
          position = stream.position;
        } else if (!stream.seekable) {
          throw new FS.ErrnoError(70);
        }
        var bytesRead = stream.stream_ops.read(stream, buffer, offset, length, position);
        if (!seeking) stream.position += bytesRead;
        return bytesRead;
      },
  write(stream, buffer, offset, length, position, canOwn) {
        if (length < 0 || position < 0) {
          throw new FS.ErrnoError(28);
        }
        if (FS.isClosed(stream)) {
          throw new FS.ErrnoError(8);
        }
        if ((stream.flags & 2097155) === 0) {
          throw new FS.ErrnoError(8);
        }
        if (FS.isDir(stream.node.mode)) {
          throw new FS.ErrnoError(31);
        }
        if (!stream.stream_ops.write) {
          throw new FS.ErrnoError(28);
        }
        if (stream.seekable && stream.flags & 1024) {
          // seek to the end before writing in append mode
          FS.llseek(stream, 0, 2);
        }
        var seeking = typeof position != 'undefined';
        if (!seeking) {
          position = stream.position;
        } else if (!stream.seekable) {
          throw new FS.ErrnoError(70);
        }
        var bytesWritten = stream.stream_ops.write(stream, buffer, offset, length, position, canOwn);
        if (!seeking) stream.position += bytesWritten;
        return bytesWritten;
      },
  mmap(stream, length, position, prot, flags) {
        // User requests writing to file (prot & PROT_WRITE != 0).
        // Checking if we have permissions to write to the file unless
        // MAP_PRIVATE flag is set. According to POSIX spec it is possible
        // to write to file opened in read-only mode with MAP_PRIVATE flag,
        // as all modifications will be visible only in the memory of
        // the current process.
        if ((prot & 2) !== 0
            && (flags & 2) === 0
            && (stream.flags & 2097155) !== 2) {
          throw new FS.ErrnoError(2);
        }
        if ((stream.flags & 2097155) === 1) {
          throw new FS.ErrnoError(2);
        }
        if (!stream.stream_ops.mmap) {
          throw new FS.ErrnoError(43);
        }
        if (!length) {
          throw new FS.ErrnoError(28);
        }
        return stream.stream_ops.mmap(stream, length, position, prot, flags);
      },
  msync(stream, buffer, offset, length, mmapFlags) {
        if (!stream.stream_ops.msync) {
          return 0;
        }
        return stream.stream_ops.msync(stream, buffer, offset, length, mmapFlags);
      },
  ioctl(stream, cmd, arg) {
        if (!stream.stream_ops.ioctl) {
          throw new FS.ErrnoError(59);
        }
        return stream.stream_ops.ioctl(stream, cmd, arg);
      },
  readFile(path, opts = {}) {
        opts.flags = opts.flags || 0;
        opts.encoding = opts.encoding || 'binary';
        if (opts.encoding !== 'utf8' && opts.encoding !== 'binary') {
          abort(`Invalid encoding type "${opts.encoding}"`);
        }
        var stream = FS.open(path, opts.flags);
        var stat = FS.stat(path);
        var length = stat.size;
        var buf = new Uint8Array(length);
        FS.read(stream, buf, 0, length, 0);
        if (opts.encoding === 'utf8') {
          buf = UTF8ArrayToString(buf);
        }
        FS.close(stream);
        return buf;
      },
  writeFile(path, data, opts = {}) {
        opts.flags = opts.flags || 577;
        var stream = FS.open(path, opts.flags, opts.mode);
        data = FS_fileDataToTypedArray(data);
        FS.write(stream, data, 0, data.byteLength, undefined, opts.canOwn);
        FS.close(stream);
      },
  cwd:() => FS.currentPath,
  chdir(path) {
        var lookup = FS.lookupPath(path, { follow: true });
        if (lookup.node === null) {
          throw new FS.ErrnoError(44);
        }
        if (!FS.isDir(lookup.node.mode)) {
          throw new FS.ErrnoError(54);
        }
        var errCode = FS.nodePermissions(lookup.node, 'x');
        if (errCode) {
          throw new FS.ErrnoError(errCode);
        }
        FS.currentPath = lookup.path;
      },
  createDefaultDirectories() {
        FS.mkdir('/tmp');
        FS.mkdir('/home');
        FS.mkdir('/home/web_user');
      },
  createDefaultDevices() {
        // create /dev
        FS.mkdir('/dev');
        // setup /dev/null
        FS.registerDevice(FS.makedev(1, 3), {
          read: () => 0,
          write: (stream, buffer, offset, length, pos) => length,
          llseek: () => 0,
        });
        FS.mkdev('/dev/null', FS.makedev(1, 3));
        // setup /dev/tty and /dev/tty1
        // stderr needs to print output using err() rather than out()
        // so we register a second tty just for it.
        TTY.register(FS.makedev(5, 0), TTY.default_tty_ops);
        TTY.register(FS.makedev(6, 0), TTY.default_tty1_ops);
        FS.mkdev('/dev/tty', FS.makedev(5, 0));
        FS.mkdev('/dev/tty1', FS.makedev(6, 0));
        // setup /dev/[u]random
        // use a buffer to avoid overhead of individual crypto calls per byte
        var randomBuffer = new Uint8Array(1024), randomLeft = 0;
        var randomByte = () => {
          if (randomLeft === 0) {
            randomFill(randomBuffer);
            randomLeft = randomBuffer.byteLength;
          }
          return randomBuffer[--randomLeft];
        };
        FS.createDevice('/dev', 'random', randomByte);
        FS.createDevice('/dev', 'urandom', randomByte);
        // we're not going to emulate the actual shm device,
        // just create the tmp dirs that reside in it commonly
        FS.mkdir('/dev/shm');
        FS.mkdir('/dev/shm/tmp');
      },
  createSpecialDirectories() {
        // create /proc/self/fd which allows /proc/self/fd/6 => readlink gives the
        // name of the stream for fd 6 (see test_unistd_ttyname)
        FS.mkdir('/proc');
        var proc_self = FS.mkdir('/proc/self');
        FS.mkdir('/proc/self/fd');
        FS.mount({
          mount() {
            var node = FS.createNode(proc_self, 'fd', 16895, 73);
            node.stream_ops = {
              llseek: MEMFS.stream_ops.llseek,
            };
            node.node_ops = {
              lookup(parent, name) {
                var fd = +name;
                var stream = FS.getStreamChecked(fd);
                var ret = {
                  parent: null,
                  mount: { mountpoint: 'fake' },
                  node_ops: { readlink: () => stream.path },
                  id: fd + 1,
                };
                ret.parent = ret; // make it look like a simple root node
                return ret;
              },
              readdir() {
                return Array.from(FS.streams.entries())
                  .filter(([k, v]) => v)
                  .map(([k, v]) => k.toString());
              }
            };
            return node;
          }
        }, {}, '/proc/self/fd');
      },
  createStandardStreams(input, output, error) {
        // TODO deprecate the old functionality of a single
        // input / output callback and that utilizes FS.createDevice
        // and instead require a unique set of stream ops
  
        // by default, we symlink the standard streams to the
        // default tty devices. however, if the standard streams
        // have been overwritten we create a unique device for
        // them instead.
        if (input) {
          FS.createDevice('/dev', 'stdin', input);
        } else {
          FS.symlink('/dev/tty', '/dev/stdin');
        }
        if (output) {
          FS.createDevice('/dev', 'stdout', null, output);
        } else {
          FS.symlink('/dev/tty', '/dev/stdout');
        }
        if (error) {
          FS.createDevice('/dev', 'stderr', null, error);
        } else {
          FS.symlink('/dev/tty1', '/dev/stderr');
        }
  
        // open default streams for the stdin, stdout and stderr devices
        var stdin = FS.open('/dev/stdin', 0);
        var stdout = FS.open('/dev/stdout', 1);
        var stderr = FS.open('/dev/stderr', 1);
      },
  staticInit() {
        FS.nameTable = new Array(4096);
  
        FS.mount(MEMFS, {}, '/');
  
        FS.createDefaultDirectories();
        FS.createDefaultDevices();
        FS.createSpecialDirectories();
  
        FS.filesystems = {
          'MEMFS': MEMFS,
        };
      },
  init(input, output, error) {
        FS.initialized = true;
  
        // Allow Module.stdin etc. to provide defaults, if none explicitly passed to us here
        input ??= Module['stdin'];
        output ??= Module['stdout'];
        error ??= Module['stderr'];
  
        FS.createStandardStreams(input, output, error);
      },
  quit() {
        FS.initialized = false;
        // force-flush all streams, so we get musl std streams printed out
        // close all of our streams
        for (var stream of FS.streams) {
          if (stream) {
            FS.close(stream);
          }
        }
      },
  findObject(path, dontResolveLastLink) {
        var ret = FS.analyzePath(path, dontResolveLastLink);
        if (!ret.exists) {
          return null;
        }
        return ret.object;
      },
  analyzePath(path, dontResolveLastLink) {
        // operate from within the context of the symlink's target
        try {
          var lookup = FS.lookupPath(path, { follow: !dontResolveLastLink });
          path = lookup.path;
        } catch (e) {
        }
        var ret = {
          isRoot: false, exists: false, error: 0, name: null, path: null, object: null,
          parentExists: false, parentPath: null, parentObject: null
        };
        try {
          var lookup = FS.lookupPath(path, { parent: true });
          ret.parentExists = true;
          ret.parentPath = lookup.path;
          ret.parentObject = lookup.node;
          ret.name = PATH.basename(path);
          lookup = FS.lookupPath(path, { follow: !dontResolveLastLink });
          ret.exists = true;
          ret.path = lookup.path;
          ret.object = lookup.node;
          ret.name = lookup.node.name;
          ret.isRoot = lookup.path === '/';
        } catch (e) {
          ret.error = e.errno;
        };
        return ret;
      },
  createPath(parent, path, canRead, canWrite) {
        parent = typeof parent == 'string' ? parent : FS.getPath(parent);
        var parts = path.split('/').reverse();
        while (parts.length) {
          var part = parts.pop();
          if (!part) continue;
          var current = PATH.join2(parent, part);
          try {
            FS.mkdir(current);
          } catch (e) {
            if (e.errno != 20) throw e;
          }
          parent = current;
        }
        return current;
      },
  createFile(parent, name, properties, canRead, canWrite) {
        var path = PATH.join2(typeof parent == 'string' ? parent : FS.getPath(parent), name);
        var mode = FS_getMode(canRead, canWrite);
        return FS.create(path, mode);
      },
  createDataFile(parent, name, data, canRead, canWrite, canOwn) {
        var path = name;
        if (parent) {
          parent = typeof parent == 'string' ? parent : FS.getPath(parent);
          path = name ? PATH.join2(parent, name) : parent;
        }
        var mode = FS_getMode(canRead, canWrite);
        var node = FS.create(path, mode);
        if (data) {
          data = FS_fileDataToTypedArray(data);
          // make sure we can write to the file
          FS.chmod(node, mode | 146);
          var stream = FS.open(node, 577);
          FS.write(stream, data, 0, data.length, 0, canOwn);
          FS.close(stream);
          FS.chmod(node, mode);
        }
      },
  createDevice(parent, name, input, output) {
        var path = PATH.join2(typeof parent == 'string' ? parent : FS.getPath(parent), name);
        var mode = FS_getMode(!!input, !!output);
        FS.createDevice.major ??= 64;
        var dev = FS.makedev(FS.createDevice.major++, 0);
        // Create a fake device that a set of stream ops to emulate
        // the old behavior.
        FS.registerDevice(dev, {
          open(stream) {
            stream.seekable = false;
          },
          close(stream) {
            // flush any pending line data
            if (output?.buffer?.length) {
              output(10);
            }
          },
          read(stream, buffer, offset, length, pos /* ignored */) {
            var bytesRead = 0;
            for (var i = 0; i < length; i++) {
              var result;
              try {
                result = input();
              } catch (e) {
                throw new FS.ErrnoError(29);
              }
              if (result === undefined && bytesRead === 0) {
                throw new FS.ErrnoError(6);
              }
              if (result === null || result === undefined) break;
              bytesRead++;
              buffer[offset+i] = result;
            }
            if (bytesRead) {
              stream.node.atime = Date.now();
            }
            return bytesRead;
          },
          write(stream, buffer, offset, length, pos) {
            for (var i = 0; i < length; i++) {
              try {
                output(buffer[offset+i]);
              } catch (e) {
                throw new FS.ErrnoError(29);
              }
            }
            if (length) {
              stream.node.mtime = stream.node.ctime = Date.now();
            }
            return i;
          }
        });
        return FS.mkdev(path, mode, dev);
      },
  forceLoadFile(obj) {
        if (obj.isDevice || obj.isFolder || obj.link || obj.contents) return true;
        if (globalThis.XMLHttpRequest) {
          abort("Lazy loading should have been performed (contents set) in createLazyFile, but it was not. Lazy loading only works in web workers. Use --embed-file or --preload-file in emcc on the main thread.");
        } else { // Command-line.
          try {
            obj.contents = readBinary(obj.url);
          } catch (e) {
            throw new FS.ErrnoError(29);
          }
        }
      },
  createLazyFile(parent, name, url, canRead, canWrite) {
        // Lazy chunked Uint8Array (implements get and length from Uint8Array).
        // Actual getting is abstracted away for eventual reuse.
        class LazyUint8Array {
          lengthKnown = false;
          chunks = []; // Loaded chunks. Index is the chunk number
          get(idx) {
            if (idx > this.length-1 || idx < 0) {
              return undefined;
            }
            var chunkOffset = idx % this.chunkSize;
            var chunkNum = (idx / this.chunkSize)|0;
            return this.getter(chunkNum)[chunkOffset];
          }
          setDataGetter(getter) {
            this.getter = getter;
          }
          cacheLength() {
            // Find length
            var xhr = new XMLHttpRequest();
            xhr.open('HEAD', url, false);
            xhr.send(null);
            if (!(xhr.status >= 200 && xhr.status < 300 || xhr.status === 304)) abort("Couldn't load " + url + ". Status: " + xhr.status);
            var datalength = Number(xhr.getResponseHeader("Content-length"));
            var header;
            var hasByteServing = (header = xhr.getResponseHeader("Accept-Ranges")) && header === "bytes";
            var usesGzip = (header = xhr.getResponseHeader("Content-Encoding")) && header === "gzip";
  
            var chunkSize = 1024*1024; // Chunk size in bytes
  
            if (!hasByteServing) chunkSize = datalength;
  
            // Function to get a range from the remote URL.
            var doXHR = (from, to) => {
              if (from > to) abort("invalid range (" + from + ", " + to + ") or no bytes requested!");
              if (to > datalength-1) abort("only " + datalength + " bytes available! programmer error!");
  
              // TODO: Use mozResponseArrayBuffer, responseStream, etc. if available.
              var xhr = new XMLHttpRequest();
              xhr.open('GET', url, false);
              if (datalength !== chunkSize) xhr.setRequestHeader("Range", "bytes=" + from + "-" + to);
  
              // Some hints to the browser that we want binary data.
              xhr.responseType = 'arraybuffer';
              if (xhr.overrideMimeType) {
                xhr.overrideMimeType('text/plain; charset=x-user-defined');
              }
  
              xhr.send(null);
              if (!(xhr.status >= 200 && xhr.status < 300 || xhr.status === 304)) abort("Couldn't load " + url + ". Status: " + xhr.status);
              if (xhr.response !== undefined) {
                return new Uint8Array(/** @type{Array<number>} */(xhr.response || []));
              }
              return intArrayFromString(xhr.responseText || '', true);
            };
            var lazyArray = this;
            lazyArray.setDataGetter((chunkNum) => {
              var start = chunkNum * chunkSize;
              var end = (chunkNum+1) * chunkSize - 1; // including this byte
              end = Math.min(end, datalength-1); // if datalength-1 is selected, this is the last block
              if (typeof lazyArray.chunks[chunkNum] == 'undefined') {
                lazyArray.chunks[chunkNum] = doXHR(start, end);
              }
              if (typeof lazyArray.chunks[chunkNum] == 'undefined') abort('doXHR failed!');
              return lazyArray.chunks[chunkNum];
            });
  
            if (usesGzip || !datalength) {
              // if the server uses gzip or doesn't supply the length, we have to download the whole file to get the (uncompressed) length
              chunkSize = datalength = 1; // this will force getter(0)/doXHR do download the whole file
              datalength = this.getter(0).length;
              chunkSize = datalength;
              out("LazyFiles on gzip forces download of the whole file when length is accessed");
            }
  
            this._length = datalength;
            this._chunkSize = chunkSize;
            this.lengthKnown = true;
          }
          get length() {
            if (!this.lengthKnown) {
              this.cacheLength();
            }
            return this._length;
          }
          get chunkSize() {
            if (!this.lengthKnown) {
              this.cacheLength();
            }
            return this._chunkSize;
          }
        }
  
        if (globalThis.XMLHttpRequest) {
          if (!ENVIRONMENT_IS_WORKER) abort('Cannot do synchronous binary XHRs outside webworkers in modern browsers. Use --embed-file or --preload-file in emcc');
          var lazyArray = new LazyUint8Array();
          var properties = { isDevice: false, contents: lazyArray };
        } else {
          var properties = { isDevice: false, url: url };
        }
  
        var node = FS.createFile(parent, name, properties, canRead, canWrite);
        // This is a total hack, but I want to get this lazy file code out of the
        // core of MEMFS. If we want to keep this lazy file concept I feel it should
        // be its own thin LAZYFS proxying calls to MEMFS.
        if (properties.contents) {
          node.contents = properties.contents;
        } else if (properties.url) {
          node.contents = null;
          node.url = properties.url;
        }
        // Add a function that defers querying the file size until it is asked the first time.
        Object.defineProperties(node, {
          usedBytes: {
            get: function() { return this.contents.length; }
          }
        });
        // override each stream op with one that tries to force load the lazy file first
        var stream_ops = {};
        for (const [key, fn] of Object.entries(node.stream_ops)) {
          stream_ops[key] = (...args) => {
            FS.forceLoadFile(node);
            return fn(...args);
          };
        }
        function writeChunks(stream, buffer, offset, length, position) {
          var contents = stream.node.contents;
          if (position >= contents.length)
            return 0;
          var size = Math.min(contents.length - position, length);
          if (contents.slice) { // normal array
            for (var i = 0; i < size; i++) {
              buffer[offset + i] = contents[position + i];
            }
          } else {
            for (var i = 0; i < size; i++) { // LazyUint8Array from sync binary XHR
              buffer[offset + i] = contents.get(position + i);
            }
          }
          return size;
        }
        // use a custom read function
        stream_ops.read = (stream, buffer, offset, length, position) => {
          FS.forceLoadFile(node);
          return writeChunks(stream, buffer, offset, length, position)
        };
        // use a custom mmap function
        stream_ops.mmap = (stream, length, position, prot, flags) => {
          FS.forceLoadFile(node);
          var ptr = mmapAlloc(length);
          if (!ptr) {
            throw new FS.ErrnoError(48);
          }
          writeChunks(stream, HEAP8, ptr, length, position);
          return { ptr, allocated: true };
        };
        node.stream_ops = stream_ops;
        return node;
      },
  };
  
  
    /**
   * Given a pointer 'ptr' to a null-terminated UTF8-encoded string in the
   * emscripten HEAP, returns a copy of that string as a Javascript String object.
   *
   * @param {number} ptr
   * @param {number=} maxBytesToRead - An optional length that specifies the
   *   maximum number of bytes to read. You can omit this parameter to scan the
   *   string until the first 0 byte. If maxBytesToRead is passed, and the string
   *   at [ptr, ptr+maxBytesToReadr[ contains a null byte in the middle, then the
   *   string will cut short at that byte index.
   * @param {boolean=} ignoreNul - If true, the function will not stop on a NUL character.
   * @return {string}
   */
  var UTF8ToString = (ptr, maxBytesToRead, ignoreNul) => {
      return ptr ? UTF8ArrayToString(HEAPU8, ptr, maxBytesToRead, ignoreNul) : '';
    };
  var SYSCALLS = {
  currentUmask:18,
  calculateAt(dirfd, path, allowEmpty) {
        if (PATH.isAbs(path)) {
          return path;
        }
        // relative path
        var dir;
        if (dirfd === -100) {
          dir = FS.cwd();
        } else {
          var dirstream = SYSCALLS.getStreamFromFD(dirfd);
          dir = dirstream.path;
        }
        if (path.length == 0) {
          if (!allowEmpty) {
            throw new FS.ErrnoError(44);;
          }
          return dir;
        }
        return dir + '/' + path;
      },
  writeStat(buf, stat) {
        HEAPU32[((buf)>>2)] = stat.dev;
        HEAPU32[(((buf)+(4))>>2)] = stat.mode;
        HEAPU32[(((buf)+(8))>>2)] = stat.nlink;
        HEAPU32[(((buf)+(12))>>2)] = stat.uid;
        HEAPU32[(((buf)+(16))>>2)] = stat.gid;
        HEAPU32[(((buf)+(20))>>2)] = stat.rdev;
        HEAP64[(((buf)+(24))>>3)] = BigInt(stat.size);
        HEAP32[(((buf)+(32))>>2)] = 4096;
        HEAP32[(((buf)+(36))>>2)] = stat.blocks;
        var atime = stat.atime.getTime();
        var mtime = stat.mtime.getTime();
        var ctime = stat.ctime.getTime();
        HEAP64[(((buf)+(40))>>3)] = BigInt(Math.floor(atime / 1000));
        HEAPU32[(((buf)+(48))>>2)] = (atime % 1000) * 1000 * 1000;
        HEAP64[(((buf)+(56))>>3)] = BigInt(Math.floor(mtime / 1000));
        HEAPU32[(((buf)+(64))>>2)] = (mtime % 1000) * 1000 * 1000;
        HEAP64[(((buf)+(72))>>3)] = BigInt(Math.floor(ctime / 1000));
        HEAPU32[(((buf)+(80))>>2)] = (ctime % 1000) * 1000 * 1000;
        HEAP64[(((buf)+(88))>>3)] = BigInt(stat.ino);
        return 0;
      },
  writeStatFs(buf, stats) {
        HEAPU32[(((buf)+(4))>>2)] = stats.bsize;
        HEAPU32[(((buf)+(60))>>2)] = stats.bsize;
        HEAP64[(((buf)+(8))>>3)] = BigInt(stats.blocks);
        HEAP64[(((buf)+(16))>>3)] = BigInt(stats.bfree);
        HEAP64[(((buf)+(24))>>3)] = BigInt(stats.bavail);
        HEAP64[(((buf)+(32))>>3)] = BigInt(stats.files);
        HEAP64[(((buf)+(40))>>3)] = BigInt(stats.ffree);
        HEAPU32[(((buf)+(48))>>2)] = stats.fsid;
        HEAPU32[(((buf)+(64))>>2)] = stats.flags;  // ST_NOSUID
        HEAPU32[(((buf)+(56))>>2)] = stats.namelen;
      },
  doMsync(addr, stream, len, flags, offset) {
        if (!FS.isFile(stream.node.mode)) {
          throw new FS.ErrnoError(43);
        }
        if (flags & 2) {
          // MAP_PRIVATE calls need not to be synced back to underlying fs
          return 0;
        }
        var buffer = HEAPU8.slice(addr, addr + len);
        FS.msync(stream, buffer, offset, len, flags);
      },
  getStreamFromFD(fd) {
        var stream = FS.getStreamChecked(fd);
        return stream;
      },
  varargs:undefined,
  getStr(ptr) {
        var ret = UTF8ToString(ptr);
        return ret;
      },
  };
  function ___syscall_fcntl64(fd, cmd, varargs) {
  SYSCALLS.varargs = varargs;
  try {
  
      var stream = SYSCALLS.getStreamFromFD(fd);
      switch (cmd) {
        case 0: {
          var arg = syscallGetVarargI();
          if (arg < 0) {
            return -28;
          }
          while (FS.streams[arg]) {
            arg++;
          }
          var newStream;
          newStream = FS.dupStream(stream, arg);
          return newStream.fd;
        }
        case 1:
        case 2:
          return 0;  // FD_CLOEXEC makes no sense for a single process.
        case 3:
          return stream.flags;
        case 4: {
          var arg = syscallGetVarargI();
          var mask = 289792;
          stream.flags = (stream.flags & ~mask) | (arg & mask);
          return 0;
        }
        case 12: {
          var arg = syscallGetVarargP();
          var offset = 0;
          // We're always unlocked.
          HEAP16[(((arg)+(offset))>>1)] = 2;
          return 0;
        }
        case 13:
        case 14:
          // Pretend that the locking is successful. These are process-level locks,
          // and Emscripten programs are a single process. If we supported linking a
          // filesystem between programs, we'd need to do more here.
          // See https://github.com/emscripten-core/emscripten/issues/23697
          return 0;
      }
      return -28;
    } catch (e) {
    if (typeof FS == 'undefined' || !(e.name === 'ErrnoError')) throw e;
    return -e.errno;
  }
  }
  

  var INT53_MAX = 9007199254740992;
  
  var INT53_MIN = -9007199254740992;
  var bigintToI53Checked = (num) => (num < INT53_MIN || num > INT53_MAX) ? NaN : Number(num);
  function ___syscall_ftruncate64(fd, length) {
    length = bigintToI53Checked(length);
  
  
  try {
  
      if (isNaN(length)) return -22;
      FS.ftruncate(fd, length);
      return 0;
    } catch (e) {
    if (typeof FS == 'undefined' || !(e.name === 'ErrnoError')) throw e;
    return -e.errno;
  }
  ;
  }

  
  function ___syscall_ioctl(fd, op, varargs) {
  SYSCALLS.varargs = varargs;
  try {
  
      var stream = SYSCALLS.getStreamFromFD(fd);
      switch (op) {
        case 21509: {
          if (!stream.tty) return -59;
          return 0;
        }
        case 21505: {
          if (!stream.tty) return -59;
          if (stream.tty.ops.ioctl_tcgets) {
            var termios = stream.tty.ops.ioctl_tcgets(stream);
            var argp = syscallGetVarargP();
            HEAP32[((argp)>>2)] = termios.c_iflag || 0;
            HEAP32[(((argp)+(4))>>2)] = termios.c_oflag || 0;
            HEAP32[(((argp)+(8))>>2)] = termios.c_cflag || 0;
            HEAP32[(((argp)+(12))>>2)] = termios.c_lflag || 0;
            for (var i = 0; i < 32; i++) {
              HEAP8[(argp + i)+(17)] = termios.c_cc[i] || 0;
            }
            return 0;
          }
          return 0;
        }
        case 21510:
        case 21511:
        case 21512: {
          if (!stream.tty) return -59;
          return 0; // no-op, not actually adjusting terminal settings
        }
        case 21506:
        case 21507:
        case 21508: {
          if (!stream.tty) return -59;
          if (stream.tty.ops.ioctl_tcsets) {
            var argp = syscallGetVarargP();
            var c_iflag = HEAP32[((argp)>>2)];
            var c_oflag = HEAP32[(((argp)+(4))>>2)];
            var c_cflag = HEAP32[(((argp)+(8))>>2)];
            var c_lflag = HEAP32[(((argp)+(12))>>2)];
            var c_cc = []
            for (var i = 0; i < 32; i++) {
              c_cc.push(HEAP8[(argp + i)+(17)]);
            }
            return stream.tty.ops.ioctl_tcsets(stream.tty, op, { c_iflag, c_oflag, c_cflag, c_lflag, c_cc });
          }
          return 0; // no-op, not actually adjusting terminal settings
        }
        case 21519: {
          if (!stream.tty) return -59;
          var argp = syscallGetVarargP();
          HEAP32[((argp)>>2)] = 0;
          return 0;
        }
        case 21520: {
          if (!stream.tty) return -59;
          return -28; // not supported
        }
        case 21537:
        case 21531: {
          var argp = syscallGetVarargP();
          return FS.ioctl(stream, op, argp);
        }
        case 21523: {
          // TODO: in theory we should write to the winsize struct that gets
          // passed in, but for now musl doesn't read anything on it
          if (!stream.tty) return -59;
          if (stream.tty.ops.ioctl_tiocgwinsz) {
            var winsize = stream.tty.ops.ioctl_tiocgwinsz(stream.tty);
            var argp = syscallGetVarargP();
            HEAP16[((argp)>>1)] = winsize[0];
            HEAP16[(((argp)+(2))>>1)] = winsize[1];
          }
          return 0;
        }
        case 21524: {
          // TODO: technically, this ioctl call should change the window size.
          // but, since emscripten doesn't have any concept of a terminal window
          // yet, we'll just silently throw it away as we do TIOCGWINSZ
          if (!stream.tty) return -59;
          return 0;
        }
        case 21515: {
          if (!stream.tty) return -59;
          return 0;
        }
        default: return -28; // not supported
      }
    } catch (e) {
    if (typeof FS == 'undefined' || !(e.name === 'ErrnoError')) throw e;
    return -e.errno;
  }
  }
  

  function ___syscall_mkdirat(dirfd, path, mode) {
  try {
  
      path = SYSCALLS.getStr(path);
      path = SYSCALLS.calculateAt(dirfd, path);
      mode &= ~SYSCALLS.currentUmask;
      FS.mkdir(path, mode, 0);
      return 0;
    } catch (e) {
    if (typeof FS == 'undefined' || !(e.name === 'ErrnoError')) throw e;
    return -e.errno;
  }
  }
  

  
  function ___syscall_openat(dirfd, path, flags, varargs) {
  SYSCALLS.varargs = varargs;
  try {
  
      path = SYSCALLS.getStr(path);
      path = SYSCALLS.calculateAt(dirfd, path);
      var mode = varargs ? syscallGetVarargI() : 0;
      if (flags & 64) {
        mode &= ~SYSCALLS.currentUmask;
      }
      return FS.open(path, flags, mode).fd;
    } catch (e) {
    if (typeof FS == 'undefined' || !(e.name === 'ErrnoError')) throw e;
    return -e.errno;
  }
  }
  

  var __abort_js = () =>
      abort('');

  var getExecutableName = () => thisProgram || './this.program';
  
  var stringToUTF8 = (str, outPtr, maxBytesToWrite) => {
      return stringToUTF8Array(str, HEAPU8, outPtr, maxBytesToWrite);
    };
  var __emscripten_get_progname = (str, len) => stringToUTF8(getExecutableName(), str, len);

  var runtimeKeepaliveCounter = 0;
  var __emscripten_runtime_keepalive_clear = () => {
      noExitRuntime = false;
      runtimeKeepaliveCounter = 0;
    };

  
  
  
  
  
  function __mmap_js(len, prot, flags, fd, offset, allocated, addr) {
    offset = bigintToI53Checked(offset);
  
  
  try {
  
      var stream = SYSCALLS.getStreamFromFD(fd);
      var res = FS.mmap(stream, len, offset, prot, flags);
      var ptr = res.ptr;
      HEAP32[((allocated)>>2)] = res.allocated;
      HEAPU32[((addr)>>2)] = ptr;
      return 0;
    } catch (e) {
    if (typeof FS == 'undefined' || !(e.name === 'ErrnoError')) throw e;
    return -e.errno;
  }
  ;
  }

  
  function __munmap_js(addr, len, prot, flags, fd, offset) {
    offset = bigintToI53Checked(offset);
  
  
  try {
  
      var stream = SYSCALLS.getStreamFromFD(fd);
      if (prot & 2) {
        SYSCALLS.doMsync(addr, stream, len, flags, offset);
      }
    } catch (e) {
    if (typeof FS == 'undefined' || !(e.name === 'ErrnoError')) throw e;
    return -e.errno;
  }
  ;
  }

  var timers = {
  };
  
  var clearTimers = () => {
      for (var t of Object.values(timers)) {
        clearTimeout(t.id);
      }
    };
  
  var handleException = (e) => {
      // Certain exception types we do not treat as errors since they are used for
      // internal control flow.
      // 1. ExitStatus, which is thrown by exit()
      // 2. "unwind", which is thrown by emscripten_unwind_to_js_event_loop() and others
      //    that wish to return to JS event loop.
      if (e instanceof ExitStatus || e == 'unwind') {
        return EXITSTATUS;
      }
      quit_(1, e);
    };
  
  
  var keepRuntimeAlive = () => noExitRuntime || runtimeKeepaliveCounter > 0;
  var _proc_exit = (code) => {
      EXITSTATUS = code;
      if (!keepRuntimeAlive()) {
        Module['onExit']?.(code);
        ABORT = true;
      }
      quit_(code, new ExitStatus(code));
    };
  /** @param {boolean|number=} implicit */
  var exitJS = (status, implicit) => {
      EXITSTATUS = status;
  
      _proc_exit(status);
    };
  var _exit = exitJS;
  
  
  var maybeExit = () => {
      if (!keepRuntimeAlive()) {
        try {
          _exit(EXITSTATUS);
        } catch (e) {
          handleException(e);
        }
      }
    };
  var callUserCallback = (func) => {
      if (ABORT) {
        return;
      }
      try {
        return func();
      } catch (e) {
        handleException(e);
      } finally {
        maybeExit();
      }
    };
  
  
  var _emscripten_get_now = () => performance.now();
  var __setitimer_js = (which, timeout_ms) => {
      // First, clear any existing timer.
      if (timers[which]) {
        clearTimeout(timers[which].id);
        delete timers[which];
      }
  
      // A timeout of zero simply cancels the current timeout so we have nothing
      // more to do.
      if (!timeout_ms) return 0;
  
      var id = setTimeout(() => {
        delete timers[which];
        callUserCallback(() => __emscripten_timeout(which, _emscripten_get_now()));
      }, timeout_ms);
      timers[which] = { id, timeout_ms };
      return 0;
    };

  var __tzset_js = (timezone, daylight, std_name, dst_name) => {
      // TODO: Use (malleable) environment variables instead of system settings.
      var currentYear = new Date().getFullYear();
      var winter = new Date(currentYear, 0, 1);
      var summer = new Date(currentYear, 6, 1);
      var winterOffset = winter.getTimezoneOffset();
      var summerOffset = summer.getTimezoneOffset();
  
      // Local standard timezone offset. Local standard time is not adjusted for
      // daylight savings.  This code uses the fact that getTimezoneOffset returns
      // a greater value during Standard Time versus Daylight Saving Time (DST).
      // Thus it determines the expected output during Standard Time, and it
      // compares whether the output of the given date the same (Standard) or less
      // (DST).
      var stdTimezoneOffset = Math.max(winterOffset, summerOffset);
  
      // timezone is specified as seconds west of UTC ("The external variable
      // `timezone` shall be set to the difference, in seconds, between
      // Coordinated Universal Time (UTC) and local standard time."), the same
      // as returned by stdTimezoneOffset.
      // See http://pubs.opengroup.org/onlinepubs/009695399/functions/tzset.html
      HEAPU32[((timezone)>>2)] = stdTimezoneOffset * 60;
  
      HEAP32[((daylight)>>2)] = Number(winterOffset != summerOffset);
  
      var extractZone = (timezoneOffset) => {
        // Why inverse sign?
        // Read here https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date/getTimezoneOffset
        var sign = timezoneOffset >= 0 ? "-" : "+";
  
        var absOffset = Math.abs(timezoneOffset)
        var hours = String(Math.floor(absOffset / 60)).padStart(2, "0");
        var minutes = String(absOffset % 60).padStart(2, "0");
  
        return `UTC${sign}${hours}${minutes}`;
      }
  
      var winterName = extractZone(winterOffset);
      var summerName = extractZone(summerOffset);
      if (summerOffset < winterOffset) {
        // Northern hemisphere
        stringToUTF8(winterName, std_name, 17);
        stringToUTF8(summerName, dst_name, 17);
      } else {
        stringToUTF8(winterName, dst_name, 17);
        stringToUTF8(summerName, std_name, 17);
      }
    };

  var getHeapMax = () =>
      // Stay one Wasm page short of 4GB: while e.g. Chrome is able to allocate
      // full 4GB Wasm memories, the size will wrap back to 0 bytes in Wasm side
      // for any code that deals with heap sizes, which would require special
      // casing all heap size related code to treat 0 specially.
      2147483648;
  
  
  var growMemory = (size) => {
      var oldHeapSize = wasmMemory.buffer.byteLength;
      var pages = ((size - oldHeapSize + 65535) / 65536) | 0;
      try {
        // round size grow request up to wasm page size (fixed 64KB per spec)
        wasmMemory.grow(pages); // .grow() takes a delta compared to the previous size
        updateMemoryViews();
        return 1 /*success*/;
      } catch(e) {
      }
      // implicit 0 return to save code size (caller will cast "undefined" into 0
      // anyhow)
    };
  var _emscripten_resize_heap = (requestedSize) => {
      var oldSize = HEAPU8.length;
      // With CAN_ADDRESS_2GB or MEMORY64, pointers are already unsigned.
      requestedSize >>>= 0;
      // With multithreaded builds, races can happen (another thread might increase the size
      // in between), so return a failure, and let the caller retry.
  
      // Memory resize rules:
      // 1.  Always increase heap size to at least the requested size, rounded up
      //     to next page multiple.
      // 2a. If MEMORY_GROWTH_LINEAR_STEP == -1, excessively resize the heap
      //     geometrically: increase the heap size according to
      //     MEMORY_GROWTH_GEOMETRIC_STEP factor (default +20%), At most
      //     overreserve by MEMORY_GROWTH_GEOMETRIC_CAP bytes (default 96MB).
      // 2b. If MEMORY_GROWTH_LINEAR_STEP != -1, excessively resize the heap
      //     linearly: increase the heap size by at least
      //     MEMORY_GROWTH_LINEAR_STEP bytes.
      // 3.  Max size for the heap is capped at 2048MB-WASM_PAGE_SIZE, or by
      //     MAXIMUM_MEMORY, or by ASAN limit, depending on which is smallest
      // 4.  If we were unable to allocate as much memory, it may be due to
      //     over-eager decision to excessively reserve due to (3) above.
      //     Hence if an allocation fails, cut down on the amount of excess
      //     growth, in an attempt to succeed to perform a smaller allocation.
  
      // A limit is set for how much we can grow. We should not exceed that
      // (the wasm binary specifies it, so if we tried, we'd fail anyhow).
      var maxHeapSize = getHeapMax();
      if (requestedSize > maxHeapSize) {
        return false;
      }
  
      // Loop through potential heap size increases. If we attempt a too eager
      // reservation that fails, cut down on the attempted size and reserve a
      // smaller bump instead. (max 3 times, chosen somewhat arbitrarily)
      for (var cutDown = 1; cutDown <= 4; cutDown *= 2) {
        var overGrownHeapSize = oldSize * (1 + 0.2 / cutDown); // ensure geometric growth
        // but limit overreserving (default to capping at +96MB overgrowth at most)
        overGrownHeapSize = Math.min(overGrownHeapSize, requestedSize + 100663296 );
  
        var newSize = Math.min(maxHeapSize, alignMemory(Math.max(requestedSize, overGrownHeapSize), 65536));
  
        var replacement = growMemory(newSize);
        if (replacement) {
  
          return true;
        }
      }
      return false;
    };

  var ENV = {
  };
  
  var getEnvStrings = () => {
      if (!getEnvStrings.strings) {
        // Default values.
        var lang = (globalThis.navigator?.language ?? 'C').replace('-', '_') + '.UTF-8';
        var env = {
          'USER': 'web_user',
          'LOGNAME': 'web_user',
          'PATH': '/',
          'PWD': '/',
          'HOME': '/home/web_user',
          'LANG': lang,
          '_': getExecutableName()
        };
        // Apply the user-provided values, if any.
        for (var x in ENV) {
          // x is a key in ENV; if ENV[x] is undefined, that means it was
          // explicitly set to be so. We allow user code to do that to
          // force variables with default values to remain unset.
          if (ENV[x] === undefined) delete env[x];
          else env[x] = ENV[x];
        }
        var strings = [];
        for (var x in env) {
          strings.push(`${x}=${env[x]}`);
        }
        getEnvStrings.strings = strings;
      }
      return getEnvStrings.strings;
    };
  
  var _environ_get = (__environ, environ_buf) => {
      var bufSize = 0;
      var envp = 0;
      for (var string of getEnvStrings()) {
        var ptr = environ_buf + bufSize;
        HEAPU32[(((__environ)+(envp))>>2)] = ptr;
        bufSize += stringToUTF8(string, ptr, Infinity) + 1;
        envp += 4;
      }
      return 0;
    };

  
  var _environ_sizes_get = (penviron_count, penviron_buf_size) => {
      var strings = getEnvStrings();
      HEAPU32[((penviron_count)>>2)] = strings.length;
      var bufSize = 0;
      for (var string of strings) {
        bufSize += lengthBytesUTF8(string) + 1;
      }
      HEAPU32[((penviron_buf_size)>>2)] = bufSize;
      return 0;
    };

  function _fd_close(fd) {
  try {
  
      var stream = SYSCALLS.getStreamFromFD(fd);
      FS.close(stream);
      return 0;
    } catch (e) {
    if (typeof FS == 'undefined' || !(e.name === 'ErrnoError')) throw e;
    return e.errno;
  }
  }
  

  /** @param {number=} offset */
  var doReadv = (stream, iov, iovcnt, offset) => {
      var ret = 0;
      for (var i = 0; i < iovcnt; i++) {
        var ptr = HEAPU32[((iov)>>2)];
        var len = HEAPU32[(((iov)+(4))>>2)];
        iov += 8;
        var curr = FS.read(stream, HEAP8, ptr, len, offset);
        if (curr < 0) return -1;
        ret += curr;
        if (curr < len) break; // nothing more to read
        if (typeof offset != 'undefined') {
          offset += curr;
        }
      }
      return ret;
    };
  
  function _fd_read(fd, iov, iovcnt, pnum) {
  try {
  
      var stream = SYSCALLS.getStreamFromFD(fd);
      var num = doReadv(stream, iov, iovcnt);
      HEAPU32[((pnum)>>2)] = num;
      return 0;
    } catch (e) {
    if (typeof FS == 'undefined' || !(e.name === 'ErrnoError')) throw e;
    return e.errno;
  }
  }
  

  
  function _fd_seek(fd, offset, whence, newOffset) {
    offset = bigintToI53Checked(offset);
  
  
  try {
  
      if (isNaN(offset)) return 22;
      var stream = SYSCALLS.getStreamFromFD(fd);
      FS.llseek(stream, offset, whence);
      HEAP64[((newOffset)>>3)] = BigInt(stream.position);
      if (stream.getdents && offset === 0 && whence === 0) stream.getdents = null; // reset readdir state
      return 0;
    } catch (e) {
    if (typeof FS == 'undefined' || !(e.name === 'ErrnoError')) throw e;
    return e.errno;
  }
  ;
  }

  /** @param {number=} offset */
  var doWritev = (stream, iov, iovcnt, offset) => {
      var ret = 0;
      for (var i = 0; i < iovcnt; i++) {
        var ptr = HEAPU32[((iov)>>2)];
        var len = HEAPU32[(((iov)+(4))>>2)];
        iov += 8;
        var curr = FS.write(stream, HEAP8, ptr, len, offset);
        if (curr < 0) return -1;
        ret += curr;
        if (curr < len) {
          // No more space to write.
          break;
        }
        if (typeof offset != 'undefined') {
          offset += curr;
        }
      }
      return ret;
    };
  
  function _fd_write(fd, iov, iovcnt, pnum) {
  try {
  
      var stream = SYSCALLS.getStreamFromFD(fd);
      var num = doWritev(stream, iov, iovcnt);
      HEAPU32[((pnum)>>2)] = num;
      return 0;
    } catch (e) {
    if (typeof FS == 'undefined' || !(e.name === 'ErrnoError')) throw e;
    return e.errno;
  }
  }
  


  var getCFunc = (ident) => {
      var func = Module['_' + ident]; // closure exported function
      return func;
    };
  
  var writeArrayToMemory = (array, buffer) => {
      HEAP8.set(array, buffer);
    };
  
  
  
  var stackAlloc = (sz) => __emscripten_stack_alloc(sz);
  var stringToUTF8OnStack = (str) => {
      var size = lengthBytesUTF8(str) + 1;
      var ret = stackAlloc(size);
      stringToUTF8(str, ret, size);
      return ret;
    };
  
  
  
  
  
    /**
   * @param {string|null=} returnType
   * @param {Array=} argTypes
   * @param {Array=} args
   * @param {Object=} opts
   */
  var ccall = (ident, returnType, argTypes, args, opts) => {
      // For fast lookup of conversion functions
      var toC = {
        'string': (str) => {
          var ret = 0;
          if (str !== null && str !== undefined && str !== 0) { // null string
            ret = stringToUTF8OnStack(str);
          }
          return ret;
        },
        'array': (arr) => {
          var ret = stackAlloc(arr.length);
          writeArrayToMemory(arr, ret);
          return ret;
        }
      };
  
      function convertReturnValue(ret) {
        if (returnType === 'string') {
          return UTF8ToString(ret);
        }
        if (returnType === 'boolean') return Boolean(ret);
        return ret;
      }
  
      var func = getCFunc(ident);
      var cArgs = [];
      var stack = 0;
      if (args) {
        for (var i = 0; i < args.length; i++) {
          var converter = toC[argTypes[i]];
          if (converter) {
            if (stack === 0) stack = stackSave();
            cArgs[i] = converter(args[i]);
          } else {
            cArgs[i] = args[i];
          }
        }
      }
      var ret = func(...cArgs);
      function onDone(ret) {
        if (stack !== 0) stackRestore(stack);
        return convertReturnValue(ret);
      }
  
      ret = onDone(ret);
      return ret;
    };







  FS.createPreloadedFile = FS_createPreloadedFile;
  FS.preloadFile = FS_preloadFile;
  FS.staticInit();;
// End JS library code

// include: postlibrary.js
// This file is included after the automatically-generated JS library code
// but before the wasm module is created.

{

  // Begin ATMODULES hooks
  if (Module['noExitRuntime']) noExitRuntime = Module['noExitRuntime'];
if (Module['preloadPlugins']) preloadPlugins = Module['preloadPlugins'];
if (Module['print']) out = Module['print'];
if (Module['printErr']) err = Module['printErr'];
if (Module['wasmBinary']) wasmBinary = Module['wasmBinary'];
  // End ATMODULES hooks

  if (Module['arguments']) arguments_ = Module['arguments'];
  if (Module['thisProgram']) thisProgram = Module['thisProgram'];

  if (Module['preInit']) {
    if (typeof Module['preInit'] == 'function') Module['preInit'] = [Module['preInit']];
    while (Module['preInit'].length > 0) {
      Module['preInit'].shift()();
    }
  }
}

// Begin runtime exports
  Module['ccall'] = ccall;
  Module['UTF8ToString'] = UTF8ToString;
  // End runtime exports
  // Begin JS library exports
  Module['ExitStatus'] = ExitStatus;
  Module['HEAP16'] = HEAP16;
  Module['HEAP32'] = HEAP32;
  Module['HEAP64'] = HEAP64;
  Module['HEAP8'] = HEAP8;
  Module['HEAPF32'] = HEAPF32;
  Module['HEAPF64'] = HEAPF64;
  Module['HEAPU16'] = HEAPU16;
  Module['HEAPU32'] = HEAPU32;
  Module['HEAPU64'] = HEAPU64;
  Module['HEAPU8'] = HEAPU8;
  Module['addOnPostRun'] = addOnPostRun;
  Module['onPostRuns'] = onPostRuns;
  Module['callRuntimeCallbacks'] = callRuntimeCallbacks;
  Module['addOnPreRun'] = addOnPreRun;
  Module['onPreRuns'] = onPreRuns;
  Module['addRunDependency'] = addRunDependency;
  Module['runDependencies'] = runDependencies;
  Module['removeRunDependency'] = removeRunDependency;
  Module['dependenciesFulfilled'] = dependenciesFulfilled;
  Module['getValue'] = getValue;
  Module['noExitRuntime'] = noExitRuntime;
  Module['setValue'] = setValue;
  Module['stackRestore'] = stackRestore;
  Module['stackSave'] = stackSave;
  Module['___call_sighandler'] = ___call_sighandler;
  Module['getWasmTableEntry'] = getWasmTableEntry;
  Module['wasmTableMirror'] = wasmTableMirror;
  Module['___syscall_fcntl64'] = ___syscall_fcntl64;
  Module['syscallGetVarargP'] = syscallGetVarargP;
  Module['syscallGetVarargI'] = syscallGetVarargI;
  Module['SYSCALLS'] = SYSCALLS;
  Module['PATH'] = PATH;
  Module['FS'] = FS;
  Module['randomFill'] = randomFill;
  Module['initRandomFill'] = initRandomFill;
  Module['PATH_FS'] = PATH_FS;
  Module['TTY'] = TTY;
  Module['UTF8ArrayToString'] = UTF8ArrayToString;
  Module['UTF8Decoder'] = UTF8Decoder;
  Module['findStringEnd'] = findStringEnd;
  Module['FS_stdin_getChar'] = FS_stdin_getChar;
  Module['FS_stdin_getChar_buffer'] = FS_stdin_getChar_buffer;
  Module['intArrayFromString'] = intArrayFromString;
  Module['lengthBytesUTF8'] = lengthBytesUTF8;
  Module['stringToUTF8Array'] = stringToUTF8Array;
  Module['MEMFS'] = MEMFS;
  Module['mmapAlloc'] = mmapAlloc;
  Module['zeroMemory'] = zeroMemory;
  Module['alignMemory'] = alignMemory;
  Module['FS_modeStringToFlags'] = FS_modeStringToFlags;
  Module['FS_fileDataToTypedArray'] = FS_fileDataToTypedArray;
  Module['FS_getMode'] = FS_getMode;
  Module['FS_createPreloadedFile'] = FS_createPreloadedFile;
  Module['FS_preloadFile'] = FS_preloadFile;
  Module['asyncLoad'] = asyncLoad;
  Module['FS_createDataFile'] = FS_createDataFile;
  Module['getUniqueRunDependency'] = getUniqueRunDependency;
  Module['FS_handledByPreloadPlugin'] = FS_handledByPreloadPlugin;
  Module['preloadPlugins'] = preloadPlugins;
  Module['UTF8ToString'] = UTF8ToString;
  Module['___syscall_ftruncate64'] = ___syscall_ftruncate64;
  Module['bigintToI53Checked'] = bigintToI53Checked;
  Module['INT53_MAX'] = INT53_MAX;
  Module['INT53_MIN'] = INT53_MIN;
  Module['___syscall_ioctl'] = ___syscall_ioctl;
  Module['___syscall_mkdirat'] = ___syscall_mkdirat;
  Module['___syscall_openat'] = ___syscall_openat;
  Module['__abort_js'] = __abort_js;
  Module['__emscripten_get_progname'] = __emscripten_get_progname;
  Module['getExecutableName'] = getExecutableName;
  Module['stringToUTF8'] = stringToUTF8;
  Module['__emscripten_runtime_keepalive_clear'] = __emscripten_runtime_keepalive_clear;
  Module['runtimeKeepaliveCounter'] = runtimeKeepaliveCounter;
  Module['__mmap_js'] = __mmap_js;
  Module['__munmap_js'] = __munmap_js;
  Module['__setitimer_js'] = __setitimer_js;
  Module['timers'] = timers;
  Module['clearTimers'] = clearTimers;
  Module['callUserCallback'] = callUserCallback;
  Module['handleException'] = handleException;
  Module['maybeExit'] = maybeExit;
  Module['_exit'] = _exit;
  Module['exitJS'] = exitJS;
  Module['_proc_exit'] = _proc_exit;
  Module['keepRuntimeAlive'] = keepRuntimeAlive;
  Module['_emscripten_get_now'] = _emscripten_get_now;
  Module['__tzset_js'] = __tzset_js;
  Module['_emscripten_resize_heap'] = _emscripten_resize_heap;
  Module['getHeapMax'] = getHeapMax;
  Module['growMemory'] = growMemory;
  Module['_environ_get'] = _environ_get;
  Module['getEnvStrings'] = getEnvStrings;
  Module['ENV'] = ENV;
  Module['_environ_sizes_get'] = _environ_sizes_get;
  Module['_fd_close'] = _fd_close;
  Module['_fd_read'] = _fd_read;
  Module['doReadv'] = doReadv;
  Module['_fd_seek'] = _fd_seek;
  Module['_fd_write'] = _fd_write;
  Module['doWritev'] = doWritev;
  Module['ccall'] = ccall;
  Module['getCFunc'] = getCFunc;
  Module['writeArrayToMemory'] = writeArrayToMemory;
  Module['stringToUTF8OnStack'] = stringToUTF8OnStack;
  Module['stackAlloc'] = stackAlloc;
  // End JS library exports

// end include: postlibrary.js


// Imports from the Wasm binary.
var _neonx_init_lut,
  _neonx_isqrt64,
  _neonx_fast_dist_fixed,
  _neonx_fast_sin_fixed,
  _neonx_get_color,
  _neonx_set_frequency,
  _neonx_set_gradient_angle,
  _neonx_set_opacity,
  _neonx_set_quantization,
  _neonx_wasm_init,
  _neonx_wasm_get_color,
  _neonx_wasm_set_frequency,
  _neonx_wasm_set_gradient_angle,
  _neonx_wasm_set_opacity,
  _neonx_wasm_set_quantization,
  _neonx_apply_colors,
  _malloc,
  _neonx_wasm_render_canvas,
  _msgs_init,
  _msgs_set_language,
  _get_msg,
  _free,
  _emscripten_stack_get_end,
  _emscripten_stack_get_base,
  _memcpy,
  _memcmp,
  __emscripten_memcpy_bulkmem,
  __emscripten_memset_bulkmem,
  _emscripten_builtin_memalign,
  _emscripten_stack_get_current,
  _calloc,
  _fileno,
  _realloc,
  _htons,
  _ntohs,
  _htonl,
  _strerror,
  __emscripten_timeout,
  _setThrew,
  __emscripten_tempret_set,
  __emscripten_tempret_get,
  ___get_temp_ret,
  ___set_temp_ret,
  _getTempRet0,
  _setTempRet0,
  ___emutls_get_address,
  _emscripten_stack_init,
  _emscripten_stack_set_limits,
  _emscripten_stack_get_free,
  __emscripten_stack_restore,
  __emscripten_stack_alloc,
  __ZNSt8bad_castD2Ev,
  __ZdlPvm,
  __Znwm,
  __ZnamSt11align_val_t,
  __ZdaPvSt11align_val_t,
  __ZNSt13runtime_errorD2Ev,
  __ZNKSt13runtime_error4whatEv,
  __ZnwmSt11align_val_t,
  __ZdlPvmSt11align_val_t,
  ___cxa_pure_virtual,
  ___cxa_uncaught_exceptions,
  ___cxa_decrement_exception_refcount,
  ___cxa_increment_exception_refcount,
  ___cxa_current_primary_exception,
  __ZSt9terminatev,
  ___cxa_rethrow_primary_exception,
  __ZNSt9exceptionD2Ev,
  __ZNSt11logic_errorD2Ev,
  __ZNKSt11logic_error4whatEv,
  __ZdaPv,
  __Znam,
  __ZSt15get_new_handlerv,
  __ZdlPv,
  __ZdaPvm,
  __ZdlPvSt11align_val_t,
  __ZdaPvmSt11align_val_t,
  ___dynamic_cast,
  ___cxa_bad_cast,
  ___cxa_bad_typeid,
  ___cxa_throw_bad_array_new_length,
  __ZSt14set_unexpectedPFvvE,
  __ZSt13set_terminatePFvvE,
  __ZSt15set_new_handlerPFvvE,
  ___cxa_demangle,
  ___cxa_guard_acquire,
  ___cxa_guard_release,
  ___cxa_guard_abort,
  __ZSt14get_unexpectedv,
  __ZSt10unexpectedv,
  __ZSt13get_terminatev,
  ___cxa_uncaught_exception,
  ___cxa_allocate_exception,
  ___cxa_free_exception,
  ___cxa_init_primary_exception,
  ___cxa_thread_atexit,
  ___cxa_deleted_virtual,
  __ZNSt9type_infoD2Ev,
  ___cxa_get_exception_ptr,
  __ZNSt9exceptionD0Ev,
  __ZNSt9exceptionD1Ev,
  __ZNKSt9exception4whatEv,
  __ZNSt13bad_exceptionD0Ev,
  __ZNSt13bad_exceptionD1Ev,
  __ZNKSt13bad_exception4whatEv,
  __ZNSt9bad_allocC2Ev,
  __ZNSt9bad_allocD0Ev,
  __ZNSt9bad_allocD1Ev,
  __ZNKSt9bad_alloc4whatEv,
  __ZNSt20bad_array_new_lengthC2Ev,
  __ZNSt20bad_array_new_lengthD0Ev,
  __ZNSt20bad_array_new_lengthD1Ev,
  __ZNKSt20bad_array_new_length4whatEv,
  __ZNSt13bad_exceptionD2Ev,
  __ZNSt9bad_allocC1Ev,
  __ZNSt9bad_allocD2Ev,
  __ZNSt20bad_array_new_lengthC1Ev,
  __ZNSt20bad_array_new_lengthD2Ev,
  __ZNSt11logic_errorD0Ev,
  __ZNSt11logic_errorD1Ev,
  __ZNSt13runtime_errorD0Ev,
  __ZNSt13runtime_errorD1Ev,
  __ZNSt12domain_errorD0Ev,
  __ZNSt12domain_errorD1Ev,
  __ZNSt16invalid_argumentD0Ev,
  __ZNSt16invalid_argumentD1Ev,
  __ZNSt12length_errorD0Ev,
  __ZNSt12length_errorD1Ev,
  __ZNSt12out_of_rangeD0Ev,
  __ZNSt12out_of_rangeD1Ev,
  __ZNSt11range_errorD0Ev,
  __ZNSt11range_errorD1Ev,
  __ZNSt14overflow_errorD0Ev,
  __ZNSt14overflow_errorD1Ev,
  __ZNSt15underflow_errorD0Ev,
  __ZNSt15underflow_errorD1Ev,
  __ZNSt12domain_errorD2Ev,
  __ZNSt16invalid_argumentD2Ev,
  __ZNSt12length_errorD2Ev,
  __ZNSt12out_of_rangeD2Ev,
  __ZNSt11range_errorD2Ev,
  __ZNSt14overflow_errorD2Ev,
  __ZNSt15underflow_errorD2Ev,
  __ZNSt9type_infoD0Ev,
  __ZNSt9type_infoD1Ev,
  __ZNSt8bad_castC2Ev,
  __ZNSt8bad_castD0Ev,
  __ZNSt8bad_castD1Ev,
  __ZNKSt8bad_cast4whatEv,
  __ZNSt10bad_typeidC2Ev,
  __ZNSt10bad_typeidD2Ev,
  __ZNSt10bad_typeidD0Ev,
  __ZNSt10bad_typeidD1Ev,
  __ZNKSt10bad_typeid4whatEv,
  __ZNSt8bad_castC1Ev,
  __ZNSt10bad_typeidC1Ev,
  memory,
  ___stack_pointer,
  __indirect_function_table,
  __ZTVN10__cxxabiv120__si_class_type_infoE,
  __ZTISt8bad_cast,
  __ZTISt13runtime_error,
  __ZTVN10__cxxabiv117__class_type_infoE,
  __ZTISt9exception,
  __ZTISt11logic_error,
  __ZTVN10__cxxabiv121__vmi_class_type_infoE,
  __ZTVSt11logic_error,
  __ZTVSt13runtime_error,
  ___cxa_unexpected_handler,
  ___cxa_terminate_handler,
  ___cxa_new_handler,
  __ZTIN10__cxxabiv116__shim_type_infoE,
  __ZTIN10__cxxabiv117__class_type_infoE,
  __ZTIN10__cxxabiv117__pbase_type_infoE,
  __ZTSDn,
  __ZTIN10__cxxabiv119__pointer_type_infoE,
  __ZTSv,
  __ZTIN10__cxxabiv120__function_type_infoE,
  __ZTIN10__cxxabiv129__pointer_to_member_type_infoE,
  __ZTISt9type_info,
  __ZTSN10__cxxabiv116__shim_type_infoE,
  __ZTSN10__cxxabiv117__class_type_infoE,
  __ZTSN10__cxxabiv117__pbase_type_infoE,
  __ZTSN10__cxxabiv119__pointer_type_infoE,
  __ZTSN10__cxxabiv120__function_type_infoE,
  __ZTSN10__cxxabiv129__pointer_to_member_type_infoE,
  __ZTVN10__cxxabiv116__shim_type_infoE,
  __ZTVN10__cxxabiv123__fundamental_type_infoE,
  __ZTIN10__cxxabiv123__fundamental_type_infoE,
  __ZTSN10__cxxabiv123__fundamental_type_infoE,
  __ZTIv,
  __ZTIPv,
  __ZTVN10__cxxabiv119__pointer_type_infoE,
  __ZTSPv,
  __ZTIPKv,
  __ZTSPKv,
  __ZTIDn,
  __ZTIPDn,
  __ZTSPDn,
  __ZTIPKDn,
  __ZTSPKDn,
  __ZTIb,
  __ZTSb,
  __ZTIPb,
  __ZTSPb,
  __ZTIPKb,
  __ZTSPKb,
  __ZTIw,
  __ZTSw,
  __ZTIPw,
  __ZTSPw,
  __ZTIPKw,
  __ZTSPKw,
  __ZTIc,
  __ZTSc,
  __ZTIPc,
  __ZTSPc,
  __ZTIPKc,
  __ZTSPKc,
  __ZTIh,
  __ZTSh,
  __ZTIPh,
  __ZTSPh,
  __ZTIPKh,
  __ZTSPKh,
  __ZTIa,
  __ZTSa,
  __ZTIPa,
  __ZTSPa,
  __ZTIPKa,
  __ZTSPKa,
  __ZTIs,
  __ZTSs,
  __ZTIPs,
  __ZTSPs,
  __ZTIPKs,
  __ZTSPKs,
  __ZTIt,
  __ZTSt,
  __ZTIPt,
  __ZTSPt,
  __ZTIPKt,
  __ZTSPKt,
  __ZTIi,
  __ZTSi,
  __ZTIPi,
  __ZTSPi,
  __ZTIPKi,
  __ZTSPKi,
  __ZTIj,
  __ZTSj,
  __ZTIPj,
  __ZTSPj,
  __ZTIPKj,
  __ZTSPKj,
  __ZTIl,
  __ZTSl,
  __ZTIPl,
  __ZTSPl,
  __ZTIPKl,
  __ZTSPKl,
  __ZTIm,
  __ZTSm,
  __ZTIPm,
  __ZTSPm,
  __ZTIPKm,
  __ZTSPKm,
  __ZTIx,
  __ZTSx,
  __ZTIPx,
  __ZTSPx,
  __ZTIPKx,
  __ZTSPKx,
  __ZTIy,
  __ZTSy,
  __ZTIPy,
  __ZTSPy,
  __ZTIPKy,
  __ZTSPKy,
  __ZTIn,
  __ZTSn,
  __ZTIPn,
  __ZTSPn,
  __ZTIPKn,
  __ZTSPKn,
  __ZTIo,
  __ZTSo,
  __ZTIPo,
  __ZTSPo,
  __ZTIPKo,
  __ZTSPKo,
  __ZTIDh,
  __ZTSDh,
  __ZTIPDh,
  __ZTSPDh,
  __ZTIPKDh,
  __ZTSPKDh,
  __ZTIf,
  __ZTSf,
  __ZTIPf,
  __ZTSPf,
  __ZTIPKf,
  __ZTSPKf,
  __ZTId,
  __ZTSd,
  __ZTIPd,
  __ZTSPd,
  __ZTIPKd,
  __ZTSPKd,
  __ZTIe,
  __ZTSe,
  __ZTIPe,
  __ZTSPe,
  __ZTIPKe,
  __ZTSPKe,
  __ZTIg,
  __ZTSg,
  __ZTIPg,
  __ZTSPg,
  __ZTIPKg,
  __ZTSPKg,
  __ZTIDu,
  __ZTSDu,
  __ZTIPDu,
  __ZTSPDu,
  __ZTIPKDu,
  __ZTSPKDu,
  __ZTIDs,
  __ZTSDs,
  __ZTIPDs,
  __ZTSPDs,
  __ZTIPKDs,
  __ZTSPKDs,
  __ZTIDi,
  __ZTSDi,
  __ZTIPDi,
  __ZTSPDi,
  __ZTIPKDi,
  __ZTSPKDi,
  __ZTVN10__cxxabiv117__array_type_infoE,
  __ZTIN10__cxxabiv117__array_type_infoE,
  __ZTSN10__cxxabiv117__array_type_infoE,
  __ZTVN10__cxxabiv120__function_type_infoE,
  __ZTVN10__cxxabiv116__enum_type_infoE,
  __ZTIN10__cxxabiv116__enum_type_infoE,
  __ZTSN10__cxxabiv116__enum_type_infoE,
  __ZTIN10__cxxabiv120__si_class_type_infoE,
  __ZTSN10__cxxabiv120__si_class_type_infoE,
  __ZTIN10__cxxabiv121__vmi_class_type_infoE,
  __ZTSN10__cxxabiv121__vmi_class_type_infoE,
  __ZTVN10__cxxabiv117__pbase_type_infoE,
  __ZTVN10__cxxabiv129__pointer_to_member_type_infoE,
  __ZTVSt9bad_alloc,
  __ZTVSt20bad_array_new_length,
  __ZTISt9bad_alloc,
  __ZTISt20bad_array_new_length,
  __ZTVSt9exception,
  __ZTSSt9exception,
  __ZTVSt13bad_exception,
  __ZTISt13bad_exception,
  __ZTSSt13bad_exception,
  __ZTSSt9bad_alloc,
  __ZTSSt20bad_array_new_length,
  __ZTVSt12domain_error,
  __ZTISt12domain_error,
  __ZTSSt12domain_error,
  __ZTSSt11logic_error,
  __ZTVSt16invalid_argument,
  __ZTISt16invalid_argument,
  __ZTSSt16invalid_argument,
  __ZTVSt12length_error,
  __ZTISt12length_error,
  __ZTSSt12length_error,
  __ZTVSt12out_of_range,
  __ZTISt12out_of_range,
  __ZTSSt12out_of_range,
  __ZTVSt11range_error,
  __ZTISt11range_error,
  __ZTSSt11range_error,
  __ZTSSt13runtime_error,
  __ZTVSt14overflow_error,
  __ZTISt14overflow_error,
  __ZTSSt14overflow_error,
  __ZTVSt15underflow_error,
  __ZTISt15underflow_error,
  __ZTSSt15underflow_error,
  __ZTVSt8bad_cast,
  __ZTVSt10bad_typeid,
  __ZTISt10bad_typeid,
  __ZTVSt9type_info,
  __ZTSSt9type_info,
  __ZTSSt8bad_cast,
  __ZTSSt10bad_typeid,
  wasmMemory,
  wasmTable;


function assignWasmExports(wasmExports) {
  _neonx_init_lut = Module['_neonx_init_lut'] = wasmExports['neonx_init_lut'];
  _neonx_isqrt64 = Module['_neonx_isqrt64'] = wasmExports['neonx_isqrt64'];
  _neonx_fast_dist_fixed = Module['_neonx_fast_dist_fixed'] = wasmExports['neonx_fast_dist_fixed'];
  _neonx_fast_sin_fixed = Module['_neonx_fast_sin_fixed'] = wasmExports['neonx_fast_sin_fixed'];
  _neonx_get_color = Module['_neonx_get_color'] = wasmExports['neonx_get_color'];
  _neonx_set_frequency = Module['_neonx_set_frequency'] = wasmExports['neonx_set_frequency'];
  _neonx_set_gradient_angle = Module['_neonx_set_gradient_angle'] = wasmExports['neonx_set_gradient_angle'];
  _neonx_set_opacity = Module['_neonx_set_opacity'] = wasmExports['neonx_set_opacity'];
  _neonx_set_quantization = Module['_neonx_set_quantization'] = wasmExports['neonx_set_quantization'];
  _neonx_wasm_init = Module['_neonx_wasm_init'] = wasmExports['neonx_wasm_init'];
  _neonx_wasm_get_color = Module['_neonx_wasm_get_color'] = wasmExports['neonx_wasm_get_color'];
  _neonx_wasm_set_frequency = Module['_neonx_wasm_set_frequency'] = wasmExports['neonx_wasm_set_frequency'];
  _neonx_wasm_set_gradient_angle = Module['_neonx_wasm_set_gradient_angle'] = wasmExports['neonx_wasm_set_gradient_angle'];
  _neonx_wasm_set_opacity = Module['_neonx_wasm_set_opacity'] = wasmExports['neonx_wasm_set_opacity'];
  _neonx_wasm_set_quantization = Module['_neonx_wasm_set_quantization'] = wasmExports['neonx_wasm_set_quantization'];
  _neonx_apply_colors = Module['_neonx_apply_colors'] = wasmExports['neonx_apply_colors'];
  _malloc = Module['_malloc'] = wasmExports['malloc'];
  _neonx_wasm_render_canvas = Module['_neonx_wasm_render_canvas'] = wasmExports['neonx_wasm_render_canvas'];
  _msgs_init = Module['_msgs_init'] = wasmExports['msgs_init'];
  _msgs_set_language = Module['_msgs_set_language'] = wasmExports['msgs_set_language'];
  _get_msg = Module['_get_msg'] = wasmExports['get_msg'];
  _free = Module['_free'] = wasmExports['free'];
  _emscripten_stack_get_end = Module['_emscripten_stack_get_end'] = wasmExports['emscripten_stack_get_end'];
  _emscripten_stack_get_base = Module['_emscripten_stack_get_base'] = wasmExports['emscripten_stack_get_base'];
  _memcpy = Module['_memcpy'] = wasmExports['memcpy'];
  _memcmp = Module['_memcmp'] = wasmExports['memcmp'];
  __emscripten_memcpy_bulkmem = Module['__emscripten_memcpy_bulkmem'] = wasmExports['_emscripten_memcpy_bulkmem'];
  __emscripten_memset_bulkmem = Module['__emscripten_memset_bulkmem'] = wasmExports['_emscripten_memset_bulkmem'];
  _emscripten_builtin_memalign = Module['_emscripten_builtin_memalign'] = wasmExports['emscripten_builtin_memalign'];
  _emscripten_stack_get_current = Module['_emscripten_stack_get_current'] = wasmExports['emscripten_stack_get_current'];
  _calloc = Module['_calloc'] = wasmExports['calloc'];
  _fileno = Module['_fileno'] = wasmExports['fileno'];
  _realloc = Module['_realloc'] = wasmExports['realloc'];
  _htons = Module['_htons'] = wasmExports['htons'];
  _ntohs = Module['_ntohs'] = wasmExports['ntohs'];
  _htonl = Module['_htonl'] = wasmExports['htonl'];
  _strerror = Module['_strerror'] = wasmExports['strerror'];
  __emscripten_timeout = Module['__emscripten_timeout'] = wasmExports['_emscripten_timeout'];
  _setThrew = Module['_setThrew'] = wasmExports['setThrew'];
  __emscripten_tempret_set = Module['__emscripten_tempret_set'] = wasmExports['_emscripten_tempret_set'];
  __emscripten_tempret_get = Module['__emscripten_tempret_get'] = wasmExports['_emscripten_tempret_get'];
  ___get_temp_ret = Module['___get_temp_ret'] = wasmExports['__get_temp_ret'];
  ___set_temp_ret = Module['___set_temp_ret'] = wasmExports['__set_temp_ret'];
  _getTempRet0 = Module['_getTempRet0'] = wasmExports['getTempRet0'];
  _setTempRet0 = Module['_setTempRet0'] = wasmExports['setTempRet0'];
  ___emutls_get_address = Module['___emutls_get_address'] = wasmExports['__emutls_get_address'];
  _emscripten_stack_init = Module['_emscripten_stack_init'] = wasmExports['emscripten_stack_init'];
  _emscripten_stack_set_limits = Module['_emscripten_stack_set_limits'] = wasmExports['emscripten_stack_set_limits'];
  _emscripten_stack_get_free = Module['_emscripten_stack_get_free'] = wasmExports['emscripten_stack_get_free'];
  __emscripten_stack_restore = Module['__emscripten_stack_restore'] = wasmExports['_emscripten_stack_restore'];
  __emscripten_stack_alloc = Module['__emscripten_stack_alloc'] = wasmExports['_emscripten_stack_alloc'];
  __ZNSt8bad_castD2Ev = Module['__ZNSt8bad_castD2Ev'] = wasmExports['_ZNSt8bad_castD2Ev'];
  __ZdlPvm = Module['__ZdlPvm'] = wasmExports['_ZdlPvm'];
  __Znwm = Module['__Znwm'] = wasmExports['_Znwm'];
  __ZnamSt11align_val_t = Module['__ZnamSt11align_val_t'] = wasmExports['_ZnamSt11align_val_t'];
  __ZdaPvSt11align_val_t = Module['__ZdaPvSt11align_val_t'] = wasmExports['_ZdaPvSt11align_val_t'];
  __ZNSt13runtime_errorD2Ev = Module['__ZNSt13runtime_errorD2Ev'] = wasmExports['_ZNSt13runtime_errorD2Ev'];
  __ZNKSt13runtime_error4whatEv = Module['__ZNKSt13runtime_error4whatEv'] = wasmExports['_ZNKSt13runtime_error4whatEv'];
  __ZnwmSt11align_val_t = Module['__ZnwmSt11align_val_t'] = wasmExports['_ZnwmSt11align_val_t'];
  __ZdlPvmSt11align_val_t = Module['__ZdlPvmSt11align_val_t'] = wasmExports['_ZdlPvmSt11align_val_t'];
  ___cxa_pure_virtual = Module['___cxa_pure_virtual'] = wasmExports['__cxa_pure_virtual'];
  ___cxa_uncaught_exceptions = Module['___cxa_uncaught_exceptions'] = wasmExports['__cxa_uncaught_exceptions'];
  ___cxa_decrement_exception_refcount = Module['___cxa_decrement_exception_refcount'] = wasmExports['__cxa_decrement_exception_refcount'];
  ___cxa_increment_exception_refcount = Module['___cxa_increment_exception_refcount'] = wasmExports['__cxa_increment_exception_refcount'];
  ___cxa_current_primary_exception = Module['___cxa_current_primary_exception'] = wasmExports['__cxa_current_primary_exception'];
  __ZSt9terminatev = Module['__ZSt9terminatev'] = wasmExports['_ZSt9terminatev'];
  ___cxa_rethrow_primary_exception = Module['___cxa_rethrow_primary_exception'] = wasmExports['__cxa_rethrow_primary_exception'];
  __ZNSt9exceptionD2Ev = Module['__ZNSt9exceptionD2Ev'] = wasmExports['_ZNSt9exceptionD2Ev'];
  __ZNSt11logic_errorD2Ev = Module['__ZNSt11logic_errorD2Ev'] = wasmExports['_ZNSt11logic_errorD2Ev'];
  __ZNKSt11logic_error4whatEv = Module['__ZNKSt11logic_error4whatEv'] = wasmExports['_ZNKSt11logic_error4whatEv'];
  __ZdaPv = Module['__ZdaPv'] = wasmExports['_ZdaPv'];
  __Znam = Module['__Znam'] = wasmExports['_Znam'];
  __ZSt15get_new_handlerv = Module['__ZSt15get_new_handlerv'] = wasmExports['_ZSt15get_new_handlerv'];
  __ZdlPv = Module['__ZdlPv'] = wasmExports['_ZdlPv'];
  __ZdaPvm = Module['__ZdaPvm'] = wasmExports['_ZdaPvm'];
  __ZdlPvSt11align_val_t = Module['__ZdlPvSt11align_val_t'] = wasmExports['_ZdlPvSt11align_val_t'];
  __ZdaPvmSt11align_val_t = Module['__ZdaPvmSt11align_val_t'] = wasmExports['_ZdaPvmSt11align_val_t'];
  ___dynamic_cast = Module['___dynamic_cast'] = wasmExports['__dynamic_cast'];
  ___cxa_bad_cast = Module['___cxa_bad_cast'] = wasmExports['__cxa_bad_cast'];
  ___cxa_bad_typeid = Module['___cxa_bad_typeid'] = wasmExports['__cxa_bad_typeid'];
  ___cxa_throw_bad_array_new_length = Module['___cxa_throw_bad_array_new_length'] = wasmExports['__cxa_throw_bad_array_new_length'];
  __ZSt14set_unexpectedPFvvE = Module['__ZSt14set_unexpectedPFvvE'] = wasmExports['_ZSt14set_unexpectedPFvvE'];
  __ZSt13set_terminatePFvvE = Module['__ZSt13set_terminatePFvvE'] = wasmExports['_ZSt13set_terminatePFvvE'];
  __ZSt15set_new_handlerPFvvE = Module['__ZSt15set_new_handlerPFvvE'] = wasmExports['_ZSt15set_new_handlerPFvvE'];
  ___cxa_demangle = Module['___cxa_demangle'] = wasmExports['__cxa_demangle'];
  ___cxa_guard_acquire = Module['___cxa_guard_acquire'] = wasmExports['__cxa_guard_acquire'];
  ___cxa_guard_release = Module['___cxa_guard_release'] = wasmExports['__cxa_guard_release'];
  ___cxa_guard_abort = Module['___cxa_guard_abort'] = wasmExports['__cxa_guard_abort'];
  __ZSt14get_unexpectedv = Module['__ZSt14get_unexpectedv'] = wasmExports['_ZSt14get_unexpectedv'];
  __ZSt10unexpectedv = Module['__ZSt10unexpectedv'] = wasmExports['_ZSt10unexpectedv'];
  __ZSt13get_terminatev = Module['__ZSt13get_terminatev'] = wasmExports['_ZSt13get_terminatev'];
  ___cxa_uncaught_exception = Module['___cxa_uncaught_exception'] = wasmExports['__cxa_uncaught_exception'];
  ___cxa_allocate_exception = Module['___cxa_allocate_exception'] = wasmExports['__cxa_allocate_exception'];
  ___cxa_free_exception = Module['___cxa_free_exception'] = wasmExports['__cxa_free_exception'];
  ___cxa_init_primary_exception = Module['___cxa_init_primary_exception'] = wasmExports['__cxa_init_primary_exception'];
  ___cxa_thread_atexit = Module['___cxa_thread_atexit'] = wasmExports['__cxa_thread_atexit'];
  ___cxa_deleted_virtual = Module['___cxa_deleted_virtual'] = wasmExports['__cxa_deleted_virtual'];
  __ZNSt9type_infoD2Ev = Module['__ZNSt9type_infoD2Ev'] = wasmExports['_ZNSt9type_infoD2Ev'];
  ___cxa_get_exception_ptr = Module['___cxa_get_exception_ptr'] = wasmExports['__cxa_get_exception_ptr'];
  __ZNSt9exceptionD0Ev = Module['__ZNSt9exceptionD0Ev'] = wasmExports['_ZNSt9exceptionD0Ev'];
  __ZNSt9exceptionD1Ev = Module['__ZNSt9exceptionD1Ev'] = wasmExports['_ZNSt9exceptionD1Ev'];
  __ZNKSt9exception4whatEv = Module['__ZNKSt9exception4whatEv'] = wasmExports['_ZNKSt9exception4whatEv'];
  __ZNSt13bad_exceptionD0Ev = Module['__ZNSt13bad_exceptionD0Ev'] = wasmExports['_ZNSt13bad_exceptionD0Ev'];
  __ZNSt13bad_exceptionD1Ev = Module['__ZNSt13bad_exceptionD1Ev'] = wasmExports['_ZNSt13bad_exceptionD1Ev'];
  __ZNKSt13bad_exception4whatEv = Module['__ZNKSt13bad_exception4whatEv'] = wasmExports['_ZNKSt13bad_exception4whatEv'];
  __ZNSt9bad_allocC2Ev = Module['__ZNSt9bad_allocC2Ev'] = wasmExports['_ZNSt9bad_allocC2Ev'];
  __ZNSt9bad_allocD0Ev = Module['__ZNSt9bad_allocD0Ev'] = wasmExports['_ZNSt9bad_allocD0Ev'];
  __ZNSt9bad_allocD1Ev = Module['__ZNSt9bad_allocD1Ev'] = wasmExports['_ZNSt9bad_allocD1Ev'];
  __ZNKSt9bad_alloc4whatEv = Module['__ZNKSt9bad_alloc4whatEv'] = wasmExports['_ZNKSt9bad_alloc4whatEv'];
  __ZNSt20bad_array_new_lengthC2Ev = Module['__ZNSt20bad_array_new_lengthC2Ev'] = wasmExports['_ZNSt20bad_array_new_lengthC2Ev'];
  __ZNSt20bad_array_new_lengthD0Ev = Module['__ZNSt20bad_array_new_lengthD0Ev'] = wasmExports['_ZNSt20bad_array_new_lengthD0Ev'];
  __ZNSt20bad_array_new_lengthD1Ev = Module['__ZNSt20bad_array_new_lengthD1Ev'] = wasmExports['_ZNSt20bad_array_new_lengthD1Ev'];
  __ZNKSt20bad_array_new_length4whatEv = Module['__ZNKSt20bad_array_new_length4whatEv'] = wasmExports['_ZNKSt20bad_array_new_length4whatEv'];
  __ZNSt13bad_exceptionD2Ev = Module['__ZNSt13bad_exceptionD2Ev'] = wasmExports['_ZNSt13bad_exceptionD2Ev'];
  __ZNSt9bad_allocC1Ev = Module['__ZNSt9bad_allocC1Ev'] = wasmExports['_ZNSt9bad_allocC1Ev'];
  __ZNSt9bad_allocD2Ev = Module['__ZNSt9bad_allocD2Ev'] = wasmExports['_ZNSt9bad_allocD2Ev'];
  __ZNSt20bad_array_new_lengthC1Ev = Module['__ZNSt20bad_array_new_lengthC1Ev'] = wasmExports['_ZNSt20bad_array_new_lengthC1Ev'];
  __ZNSt20bad_array_new_lengthD2Ev = Module['__ZNSt20bad_array_new_lengthD2Ev'] = wasmExports['_ZNSt20bad_array_new_lengthD2Ev'];
  __ZNSt11logic_errorD0Ev = Module['__ZNSt11logic_errorD0Ev'] = wasmExports['_ZNSt11logic_errorD0Ev'];
  __ZNSt11logic_errorD1Ev = Module['__ZNSt11logic_errorD1Ev'] = wasmExports['_ZNSt11logic_errorD1Ev'];
  __ZNSt13runtime_errorD0Ev = Module['__ZNSt13runtime_errorD0Ev'] = wasmExports['_ZNSt13runtime_errorD0Ev'];
  __ZNSt13runtime_errorD1Ev = Module['__ZNSt13runtime_errorD1Ev'] = wasmExports['_ZNSt13runtime_errorD1Ev'];
  __ZNSt12domain_errorD0Ev = Module['__ZNSt12domain_errorD0Ev'] = wasmExports['_ZNSt12domain_errorD0Ev'];
  __ZNSt12domain_errorD1Ev = Module['__ZNSt12domain_errorD1Ev'] = wasmExports['_ZNSt12domain_errorD1Ev'];
  __ZNSt16invalid_argumentD0Ev = Module['__ZNSt16invalid_argumentD0Ev'] = wasmExports['_ZNSt16invalid_argumentD0Ev'];
  __ZNSt16invalid_argumentD1Ev = Module['__ZNSt16invalid_argumentD1Ev'] = wasmExports['_ZNSt16invalid_argumentD1Ev'];
  __ZNSt12length_errorD0Ev = Module['__ZNSt12length_errorD0Ev'] = wasmExports['_ZNSt12length_errorD0Ev'];
  __ZNSt12length_errorD1Ev = Module['__ZNSt12length_errorD1Ev'] = wasmExports['_ZNSt12length_errorD1Ev'];
  __ZNSt12out_of_rangeD0Ev = Module['__ZNSt12out_of_rangeD0Ev'] = wasmExports['_ZNSt12out_of_rangeD0Ev'];
  __ZNSt12out_of_rangeD1Ev = Module['__ZNSt12out_of_rangeD1Ev'] = wasmExports['_ZNSt12out_of_rangeD1Ev'];
  __ZNSt11range_errorD0Ev = Module['__ZNSt11range_errorD0Ev'] = wasmExports['_ZNSt11range_errorD0Ev'];
  __ZNSt11range_errorD1Ev = Module['__ZNSt11range_errorD1Ev'] = wasmExports['_ZNSt11range_errorD1Ev'];
  __ZNSt14overflow_errorD0Ev = Module['__ZNSt14overflow_errorD0Ev'] = wasmExports['_ZNSt14overflow_errorD0Ev'];
  __ZNSt14overflow_errorD1Ev = Module['__ZNSt14overflow_errorD1Ev'] = wasmExports['_ZNSt14overflow_errorD1Ev'];
  __ZNSt15underflow_errorD0Ev = Module['__ZNSt15underflow_errorD0Ev'] = wasmExports['_ZNSt15underflow_errorD0Ev'];
  __ZNSt15underflow_errorD1Ev = Module['__ZNSt15underflow_errorD1Ev'] = wasmExports['_ZNSt15underflow_errorD1Ev'];
  __ZNSt12domain_errorD2Ev = Module['__ZNSt12domain_errorD2Ev'] = wasmExports['_ZNSt12domain_errorD2Ev'];
  __ZNSt16invalid_argumentD2Ev = Module['__ZNSt16invalid_argumentD2Ev'] = wasmExports['_ZNSt16invalid_argumentD2Ev'];
  __ZNSt12length_errorD2Ev = Module['__ZNSt12length_errorD2Ev'] = wasmExports['_ZNSt12length_errorD2Ev'];
  __ZNSt12out_of_rangeD2Ev = Module['__ZNSt12out_of_rangeD2Ev'] = wasmExports['_ZNSt12out_of_rangeD2Ev'];
  __ZNSt11range_errorD2Ev = Module['__ZNSt11range_errorD2Ev'] = wasmExports['_ZNSt11range_errorD2Ev'];
  __ZNSt14overflow_errorD2Ev = Module['__ZNSt14overflow_errorD2Ev'] = wasmExports['_ZNSt14overflow_errorD2Ev'];
  __ZNSt15underflow_errorD2Ev = Module['__ZNSt15underflow_errorD2Ev'] = wasmExports['_ZNSt15underflow_errorD2Ev'];
  __ZNSt9type_infoD0Ev = Module['__ZNSt9type_infoD0Ev'] = wasmExports['_ZNSt9type_infoD0Ev'];
  __ZNSt9type_infoD1Ev = Module['__ZNSt9type_infoD1Ev'] = wasmExports['_ZNSt9type_infoD1Ev'];
  __ZNSt8bad_castC2Ev = Module['__ZNSt8bad_castC2Ev'] = wasmExports['_ZNSt8bad_castC2Ev'];
  __ZNSt8bad_castD0Ev = Module['__ZNSt8bad_castD0Ev'] = wasmExports['_ZNSt8bad_castD0Ev'];
  __ZNSt8bad_castD1Ev = Module['__ZNSt8bad_castD1Ev'] = wasmExports['_ZNSt8bad_castD1Ev'];
  __ZNKSt8bad_cast4whatEv = Module['__ZNKSt8bad_cast4whatEv'] = wasmExports['_ZNKSt8bad_cast4whatEv'];
  __ZNSt10bad_typeidC2Ev = Module['__ZNSt10bad_typeidC2Ev'] = wasmExports['_ZNSt10bad_typeidC2Ev'];
  __ZNSt10bad_typeidD2Ev = Module['__ZNSt10bad_typeidD2Ev'] = wasmExports['_ZNSt10bad_typeidD2Ev'];
  __ZNSt10bad_typeidD0Ev = Module['__ZNSt10bad_typeidD0Ev'] = wasmExports['_ZNSt10bad_typeidD0Ev'];
  __ZNSt10bad_typeidD1Ev = Module['__ZNSt10bad_typeidD1Ev'] = wasmExports['_ZNSt10bad_typeidD1Ev'];
  __ZNKSt10bad_typeid4whatEv = Module['__ZNKSt10bad_typeid4whatEv'] = wasmExports['_ZNKSt10bad_typeid4whatEv'];
  __ZNSt8bad_castC1Ev = Module['__ZNSt8bad_castC1Ev'] = wasmExports['_ZNSt8bad_castC1Ev'];
  __ZNSt10bad_typeidC1Ev = Module['__ZNSt10bad_typeidC1Ev'] = wasmExports['_ZNSt10bad_typeidC1Ev'];
  memory = Module['memory'] = wasmMemory = wasmExports['memory'];
  ___stack_pointer = Module['___stack_pointer'] = wasmExports['__stack_pointer'];
  __indirect_function_table = Module['__indirect_function_table'] = wasmTable = wasmExports['__indirect_function_table'];
  __ZTVN10__cxxabiv120__si_class_type_infoE = Module['__ZTVN10__cxxabiv120__si_class_type_infoE'] = wasmExports['_ZTVN10__cxxabiv120__si_class_type_infoE'].value;
  __ZTISt8bad_cast = Module['__ZTISt8bad_cast'] = wasmExports['_ZTISt8bad_cast'].value;
  __ZTISt13runtime_error = Module['__ZTISt13runtime_error'] = wasmExports['_ZTISt13runtime_error'].value;
  __ZTVN10__cxxabiv117__class_type_infoE = Module['__ZTVN10__cxxabiv117__class_type_infoE'] = wasmExports['_ZTVN10__cxxabiv117__class_type_infoE'].value;
  __ZTISt9exception = Module['__ZTISt9exception'] = wasmExports['_ZTISt9exception'].value;
  __ZTISt11logic_error = Module['__ZTISt11logic_error'] = wasmExports['_ZTISt11logic_error'].value;
  __ZTVN10__cxxabiv121__vmi_class_type_infoE = Module['__ZTVN10__cxxabiv121__vmi_class_type_infoE'] = wasmExports['_ZTVN10__cxxabiv121__vmi_class_type_infoE'].value;
  __ZTVSt11logic_error = Module['__ZTVSt11logic_error'] = wasmExports['_ZTVSt11logic_error'].value;
  __ZTVSt13runtime_error = Module['__ZTVSt13runtime_error'] = wasmExports['_ZTVSt13runtime_error'].value;
  ___cxa_unexpected_handler = Module['___cxa_unexpected_handler'] = wasmExports['__cxa_unexpected_handler'].value;
  ___cxa_terminate_handler = Module['___cxa_terminate_handler'] = wasmExports['__cxa_terminate_handler'].value;
  ___cxa_new_handler = Module['___cxa_new_handler'] = wasmExports['__cxa_new_handler'].value;
  __ZTIN10__cxxabiv116__shim_type_infoE = Module['__ZTIN10__cxxabiv116__shim_type_infoE'] = wasmExports['_ZTIN10__cxxabiv116__shim_type_infoE'].value;
  __ZTIN10__cxxabiv117__class_type_infoE = Module['__ZTIN10__cxxabiv117__class_type_infoE'] = wasmExports['_ZTIN10__cxxabiv117__class_type_infoE'].value;
  __ZTIN10__cxxabiv117__pbase_type_infoE = Module['__ZTIN10__cxxabiv117__pbase_type_infoE'] = wasmExports['_ZTIN10__cxxabiv117__pbase_type_infoE'].value;
  __ZTSDn = Module['__ZTSDn'] = wasmExports['_ZTSDn'].value;
  __ZTIN10__cxxabiv119__pointer_type_infoE = Module['__ZTIN10__cxxabiv119__pointer_type_infoE'] = wasmExports['_ZTIN10__cxxabiv119__pointer_type_infoE'].value;
  __ZTSv = Module['__ZTSv'] = wasmExports['_ZTSv'].value;
  __ZTIN10__cxxabiv120__function_type_infoE = Module['__ZTIN10__cxxabiv120__function_type_infoE'] = wasmExports['_ZTIN10__cxxabiv120__function_type_infoE'].value;
  __ZTIN10__cxxabiv129__pointer_to_member_type_infoE = Module['__ZTIN10__cxxabiv129__pointer_to_member_type_infoE'] = wasmExports['_ZTIN10__cxxabiv129__pointer_to_member_type_infoE'].value;
  __ZTISt9type_info = Module['__ZTISt9type_info'] = wasmExports['_ZTISt9type_info'].value;
  __ZTSN10__cxxabiv116__shim_type_infoE = Module['__ZTSN10__cxxabiv116__shim_type_infoE'] = wasmExports['_ZTSN10__cxxabiv116__shim_type_infoE'].value;
  __ZTSN10__cxxabiv117__class_type_infoE = Module['__ZTSN10__cxxabiv117__class_type_infoE'] = wasmExports['_ZTSN10__cxxabiv117__class_type_infoE'].value;
  __ZTSN10__cxxabiv117__pbase_type_infoE = Module['__ZTSN10__cxxabiv117__pbase_type_infoE'] = wasmExports['_ZTSN10__cxxabiv117__pbase_type_infoE'].value;
  __ZTSN10__cxxabiv119__pointer_type_infoE = Module['__ZTSN10__cxxabiv119__pointer_type_infoE'] = wasmExports['_ZTSN10__cxxabiv119__pointer_type_infoE'].value;
  __ZTSN10__cxxabiv120__function_type_infoE = Module['__ZTSN10__cxxabiv120__function_type_infoE'] = wasmExports['_ZTSN10__cxxabiv120__function_type_infoE'].value;
  __ZTSN10__cxxabiv129__pointer_to_member_type_infoE = Module['__ZTSN10__cxxabiv129__pointer_to_member_type_infoE'] = wasmExports['_ZTSN10__cxxabiv129__pointer_to_member_type_infoE'].value;
  __ZTVN10__cxxabiv116__shim_type_infoE = Module['__ZTVN10__cxxabiv116__shim_type_infoE'] = wasmExports['_ZTVN10__cxxabiv116__shim_type_infoE'].value;
  __ZTVN10__cxxabiv123__fundamental_type_infoE = Module['__ZTVN10__cxxabiv123__fundamental_type_infoE'] = wasmExports['_ZTVN10__cxxabiv123__fundamental_type_infoE'].value;
  __ZTIN10__cxxabiv123__fundamental_type_infoE = Module['__ZTIN10__cxxabiv123__fundamental_type_infoE'] = wasmExports['_ZTIN10__cxxabiv123__fundamental_type_infoE'].value;
  __ZTSN10__cxxabiv123__fundamental_type_infoE = Module['__ZTSN10__cxxabiv123__fundamental_type_infoE'] = wasmExports['_ZTSN10__cxxabiv123__fundamental_type_infoE'].value;
  __ZTIv = Module['__ZTIv'] = wasmExports['_ZTIv'].value;
  __ZTIPv = Module['__ZTIPv'] = wasmExports['_ZTIPv'].value;
  __ZTVN10__cxxabiv119__pointer_type_infoE = Module['__ZTVN10__cxxabiv119__pointer_type_infoE'] = wasmExports['_ZTVN10__cxxabiv119__pointer_type_infoE'].value;
  __ZTSPv = Module['__ZTSPv'] = wasmExports['_ZTSPv'].value;
  __ZTIPKv = Module['__ZTIPKv'] = wasmExports['_ZTIPKv'].value;
  __ZTSPKv = Module['__ZTSPKv'] = wasmExports['_ZTSPKv'].value;
  __ZTIDn = Module['__ZTIDn'] = wasmExports['_ZTIDn'].value;
  __ZTIPDn = Module['__ZTIPDn'] = wasmExports['_ZTIPDn'].value;
  __ZTSPDn = Module['__ZTSPDn'] = wasmExports['_ZTSPDn'].value;
  __ZTIPKDn = Module['__ZTIPKDn'] = wasmExports['_ZTIPKDn'].value;
  __ZTSPKDn = Module['__ZTSPKDn'] = wasmExports['_ZTSPKDn'].value;
  __ZTIb = Module['__ZTIb'] = wasmExports['_ZTIb'].value;
  __ZTSb = Module['__ZTSb'] = wasmExports['_ZTSb'].value;
  __ZTIPb = Module['__ZTIPb'] = wasmExports['_ZTIPb'].value;
  __ZTSPb = Module['__ZTSPb'] = wasmExports['_ZTSPb'].value;
  __ZTIPKb = Module['__ZTIPKb'] = wasmExports['_ZTIPKb'].value;
  __ZTSPKb = Module['__ZTSPKb'] = wasmExports['_ZTSPKb'].value;
  __ZTIw = Module['__ZTIw'] = wasmExports['_ZTIw'].value;
  __ZTSw = Module['__ZTSw'] = wasmExports['_ZTSw'].value;
  __ZTIPw = Module['__ZTIPw'] = wasmExports['_ZTIPw'].value;
  __ZTSPw = Module['__ZTSPw'] = wasmExports['_ZTSPw'].value;
  __ZTIPKw = Module['__ZTIPKw'] = wasmExports['_ZTIPKw'].value;
  __ZTSPKw = Module['__ZTSPKw'] = wasmExports['_ZTSPKw'].value;
  __ZTIc = Module['__ZTIc'] = wasmExports['_ZTIc'].value;
  __ZTSc = Module['__ZTSc'] = wasmExports['_ZTSc'].value;
  __ZTIPc = Module['__ZTIPc'] = wasmExports['_ZTIPc'].value;
  __ZTSPc = Module['__ZTSPc'] = wasmExports['_ZTSPc'].value;
  __ZTIPKc = Module['__ZTIPKc'] = wasmExports['_ZTIPKc'].value;
  __ZTSPKc = Module['__ZTSPKc'] = wasmExports['_ZTSPKc'].value;
  __ZTIh = Module['__ZTIh'] = wasmExports['_ZTIh'].value;
  __ZTSh = Module['__ZTSh'] = wasmExports['_ZTSh'].value;
  __ZTIPh = Module['__ZTIPh'] = wasmExports['_ZTIPh'].value;
  __ZTSPh = Module['__ZTSPh'] = wasmExports['_ZTSPh'].value;
  __ZTIPKh = Module['__ZTIPKh'] = wasmExports['_ZTIPKh'].value;
  __ZTSPKh = Module['__ZTSPKh'] = wasmExports['_ZTSPKh'].value;
  __ZTIa = Module['__ZTIa'] = wasmExports['_ZTIa'].value;
  __ZTSa = Module['__ZTSa'] = wasmExports['_ZTSa'].value;
  __ZTIPa = Module['__ZTIPa'] = wasmExports['_ZTIPa'].value;
  __ZTSPa = Module['__ZTSPa'] = wasmExports['_ZTSPa'].value;
  __ZTIPKa = Module['__ZTIPKa'] = wasmExports['_ZTIPKa'].value;
  __ZTSPKa = Module['__ZTSPKa'] = wasmExports['_ZTSPKa'].value;
  __ZTIs = Module['__ZTIs'] = wasmExports['_ZTIs'].value;
  __ZTSs = Module['__ZTSs'] = wasmExports['_ZTSs'].value;
  __ZTIPs = Module['__ZTIPs'] = wasmExports['_ZTIPs'].value;
  __ZTSPs = Module['__ZTSPs'] = wasmExports['_ZTSPs'].value;
  __ZTIPKs = Module['__ZTIPKs'] = wasmExports['_ZTIPKs'].value;
  __ZTSPKs = Module['__ZTSPKs'] = wasmExports['_ZTSPKs'].value;
  __ZTIt = Module['__ZTIt'] = wasmExports['_ZTIt'].value;
  __ZTSt = Module['__ZTSt'] = wasmExports['_ZTSt'].value;
  __ZTIPt = Module['__ZTIPt'] = wasmExports['_ZTIPt'].value;
  __ZTSPt = Module['__ZTSPt'] = wasmExports['_ZTSPt'].value;
  __ZTIPKt = Module['__ZTIPKt'] = wasmExports['_ZTIPKt'].value;
  __ZTSPKt = Module['__ZTSPKt'] = wasmExports['_ZTSPKt'].value;
  __ZTIi = Module['__ZTIi'] = wasmExports['_ZTIi'].value;
  __ZTSi = Module['__ZTSi'] = wasmExports['_ZTSi'].value;
  __ZTIPi = Module['__ZTIPi'] = wasmExports['_ZTIPi'].value;
  __ZTSPi = Module['__ZTSPi'] = wasmExports['_ZTSPi'].value;
  __ZTIPKi = Module['__ZTIPKi'] = wasmExports['_ZTIPKi'].value;
  __ZTSPKi = Module['__ZTSPKi'] = wasmExports['_ZTSPKi'].value;
  __ZTIj = Module['__ZTIj'] = wasmExports['_ZTIj'].value;
  __ZTSj = Module['__ZTSj'] = wasmExports['_ZTSj'].value;
  __ZTIPj = Module['__ZTIPj'] = wasmExports['_ZTIPj'].value;
  __ZTSPj = Module['__ZTSPj'] = wasmExports['_ZTSPj'].value;
  __ZTIPKj = Module['__ZTIPKj'] = wasmExports['_ZTIPKj'].value;
  __ZTSPKj = Module['__ZTSPKj'] = wasmExports['_ZTSPKj'].value;
  __ZTIl = Module['__ZTIl'] = wasmExports['_ZTIl'].value;
  __ZTSl = Module['__ZTSl'] = wasmExports['_ZTSl'].value;
  __ZTIPl = Module['__ZTIPl'] = wasmExports['_ZTIPl'].value;
  __ZTSPl = Module['__ZTSPl'] = wasmExports['_ZTSPl'].value;
  __ZTIPKl = Module['__ZTIPKl'] = wasmExports['_ZTIPKl'].value;
  __ZTSPKl = Module['__ZTSPKl'] = wasmExports['_ZTSPKl'].value;
  __ZTIm = Module['__ZTIm'] = wasmExports['_ZTIm'].value;
  __ZTSm = Module['__ZTSm'] = wasmExports['_ZTSm'].value;
  __ZTIPm = Module['__ZTIPm'] = wasmExports['_ZTIPm'].value;
  __ZTSPm = Module['__ZTSPm'] = wasmExports['_ZTSPm'].value;
  __ZTIPKm = Module['__ZTIPKm'] = wasmExports['_ZTIPKm'].value;
  __ZTSPKm = Module['__ZTSPKm'] = wasmExports['_ZTSPKm'].value;
  __ZTIx = Module['__ZTIx'] = wasmExports['_ZTIx'].value;
  __ZTSx = Module['__ZTSx'] = wasmExports['_ZTSx'].value;
  __ZTIPx = Module['__ZTIPx'] = wasmExports['_ZTIPx'].value;
  __ZTSPx = Module['__ZTSPx'] = wasmExports['_ZTSPx'].value;
  __ZTIPKx = Module['__ZTIPKx'] = wasmExports['_ZTIPKx'].value;
  __ZTSPKx = Module['__ZTSPKx'] = wasmExports['_ZTSPKx'].value;
  __ZTIy = Module['__ZTIy'] = wasmExports['_ZTIy'].value;
  __ZTSy = Module['__ZTSy'] = wasmExports['_ZTSy'].value;
  __ZTIPy = Module['__ZTIPy'] = wasmExports['_ZTIPy'].value;
  __ZTSPy = Module['__ZTSPy'] = wasmExports['_ZTSPy'].value;
  __ZTIPKy = Module['__ZTIPKy'] = wasmExports['_ZTIPKy'].value;
  __ZTSPKy = Module['__ZTSPKy'] = wasmExports['_ZTSPKy'].value;
  __ZTIn = Module['__ZTIn'] = wasmExports['_ZTIn'].value;
  __ZTSn = Module['__ZTSn'] = wasmExports['_ZTSn'].value;
  __ZTIPn = Module['__ZTIPn'] = wasmExports['_ZTIPn'].value;
  __ZTSPn = Module['__ZTSPn'] = wasmExports['_ZTSPn'].value;
  __ZTIPKn = Module['__ZTIPKn'] = wasmExports['_ZTIPKn'].value;
  __ZTSPKn = Module['__ZTSPKn'] = wasmExports['_ZTSPKn'].value;
  __ZTIo = Module['__ZTIo'] = wasmExports['_ZTIo'].value;
  __ZTSo = Module['__ZTSo'] = wasmExports['_ZTSo'].value;
  __ZTIPo = Module['__ZTIPo'] = wasmExports['_ZTIPo'].value;
  __ZTSPo = Module['__ZTSPo'] = wasmExports['_ZTSPo'].value;
  __ZTIPKo = Module['__ZTIPKo'] = wasmExports['_ZTIPKo'].value;
  __ZTSPKo = Module['__ZTSPKo'] = wasmExports['_ZTSPKo'].value;
  __ZTIDh = Module['__ZTIDh'] = wasmExports['_ZTIDh'].value;
  __ZTSDh = Module['__ZTSDh'] = wasmExports['_ZTSDh'].value;
  __ZTIPDh = Module['__ZTIPDh'] = wasmExports['_ZTIPDh'].value;
  __ZTSPDh = Module['__ZTSPDh'] = wasmExports['_ZTSPDh'].value;
  __ZTIPKDh = Module['__ZTIPKDh'] = wasmExports['_ZTIPKDh'].value;
  __ZTSPKDh = Module['__ZTSPKDh'] = wasmExports['_ZTSPKDh'].value;
  __ZTIf = Module['__ZTIf'] = wasmExports['_ZTIf'].value;
  __ZTSf = Module['__ZTSf'] = wasmExports['_ZTSf'].value;
  __ZTIPf = Module['__ZTIPf'] = wasmExports['_ZTIPf'].value;
  __ZTSPf = Module['__ZTSPf'] = wasmExports['_ZTSPf'].value;
  __ZTIPKf = Module['__ZTIPKf'] = wasmExports['_ZTIPKf'].value;
  __ZTSPKf = Module['__ZTSPKf'] = wasmExports['_ZTSPKf'].value;
  __ZTId = Module['__ZTId'] = wasmExports['_ZTId'].value;
  __ZTSd = Module['__ZTSd'] = wasmExports['_ZTSd'].value;
  __ZTIPd = Module['__ZTIPd'] = wasmExports['_ZTIPd'].value;
  __ZTSPd = Module['__ZTSPd'] = wasmExports['_ZTSPd'].value;
  __ZTIPKd = Module['__ZTIPKd'] = wasmExports['_ZTIPKd'].value;
  __ZTSPKd = Module['__ZTSPKd'] = wasmExports['_ZTSPKd'].value;
  __ZTIe = Module['__ZTIe'] = wasmExports['_ZTIe'].value;
  __ZTSe = Module['__ZTSe'] = wasmExports['_ZTSe'].value;
  __ZTIPe = Module['__ZTIPe'] = wasmExports['_ZTIPe'].value;
  __ZTSPe = Module['__ZTSPe'] = wasmExports['_ZTSPe'].value;
  __ZTIPKe = Module['__ZTIPKe'] = wasmExports['_ZTIPKe'].value;
  __ZTSPKe = Module['__ZTSPKe'] = wasmExports['_ZTSPKe'].value;
  __ZTIg = Module['__ZTIg'] = wasmExports['_ZTIg'].value;
  __ZTSg = Module['__ZTSg'] = wasmExports['_ZTSg'].value;
  __ZTIPg = Module['__ZTIPg'] = wasmExports['_ZTIPg'].value;
  __ZTSPg = Module['__ZTSPg'] = wasmExports['_ZTSPg'].value;
  __ZTIPKg = Module['__ZTIPKg'] = wasmExports['_ZTIPKg'].value;
  __ZTSPKg = Module['__ZTSPKg'] = wasmExports['_ZTSPKg'].value;
  __ZTIDu = Module['__ZTIDu'] = wasmExports['_ZTIDu'].value;
  __ZTSDu = Module['__ZTSDu'] = wasmExports['_ZTSDu'].value;
  __ZTIPDu = Module['__ZTIPDu'] = wasmExports['_ZTIPDu'].value;
  __ZTSPDu = Module['__ZTSPDu'] = wasmExports['_ZTSPDu'].value;
  __ZTIPKDu = Module['__ZTIPKDu'] = wasmExports['_ZTIPKDu'].value;
  __ZTSPKDu = Module['__ZTSPKDu'] = wasmExports['_ZTSPKDu'].value;
  __ZTIDs = Module['__ZTIDs'] = wasmExports['_ZTIDs'].value;
  __ZTSDs = Module['__ZTSDs'] = wasmExports['_ZTSDs'].value;
  __ZTIPDs = Module['__ZTIPDs'] = wasmExports['_ZTIPDs'].value;
  __ZTSPDs = Module['__ZTSPDs'] = wasmExports['_ZTSPDs'].value;
  __ZTIPKDs = Module['__ZTIPKDs'] = wasmExports['_ZTIPKDs'].value;
  __ZTSPKDs = Module['__ZTSPKDs'] = wasmExports['_ZTSPKDs'].value;
  __ZTIDi = Module['__ZTIDi'] = wasmExports['_ZTIDi'].value;
  __ZTSDi = Module['__ZTSDi'] = wasmExports['_ZTSDi'].value;
  __ZTIPDi = Module['__ZTIPDi'] = wasmExports['_ZTIPDi'].value;
  __ZTSPDi = Module['__ZTSPDi'] = wasmExports['_ZTSPDi'].value;
  __ZTIPKDi = Module['__ZTIPKDi'] = wasmExports['_ZTIPKDi'].value;
  __ZTSPKDi = Module['__ZTSPKDi'] = wasmExports['_ZTSPKDi'].value;
  __ZTVN10__cxxabiv117__array_type_infoE = Module['__ZTVN10__cxxabiv117__array_type_infoE'] = wasmExports['_ZTVN10__cxxabiv117__array_type_infoE'].value;
  __ZTIN10__cxxabiv117__array_type_infoE = Module['__ZTIN10__cxxabiv117__array_type_infoE'] = wasmExports['_ZTIN10__cxxabiv117__array_type_infoE'].value;
  __ZTSN10__cxxabiv117__array_type_infoE = Module['__ZTSN10__cxxabiv117__array_type_infoE'] = wasmExports['_ZTSN10__cxxabiv117__array_type_infoE'].value;
  __ZTVN10__cxxabiv120__function_type_infoE = Module['__ZTVN10__cxxabiv120__function_type_infoE'] = wasmExports['_ZTVN10__cxxabiv120__function_type_infoE'].value;
  __ZTVN10__cxxabiv116__enum_type_infoE = Module['__ZTVN10__cxxabiv116__enum_type_infoE'] = wasmExports['_ZTVN10__cxxabiv116__enum_type_infoE'].value;
  __ZTIN10__cxxabiv116__enum_type_infoE = Module['__ZTIN10__cxxabiv116__enum_type_infoE'] = wasmExports['_ZTIN10__cxxabiv116__enum_type_infoE'].value;
  __ZTSN10__cxxabiv116__enum_type_infoE = Module['__ZTSN10__cxxabiv116__enum_type_infoE'] = wasmExports['_ZTSN10__cxxabiv116__enum_type_infoE'].value;
  __ZTIN10__cxxabiv120__si_class_type_infoE = Module['__ZTIN10__cxxabiv120__si_class_type_infoE'] = wasmExports['_ZTIN10__cxxabiv120__si_class_type_infoE'].value;
  __ZTSN10__cxxabiv120__si_class_type_infoE = Module['__ZTSN10__cxxabiv120__si_class_type_infoE'] = wasmExports['_ZTSN10__cxxabiv120__si_class_type_infoE'].value;
  __ZTIN10__cxxabiv121__vmi_class_type_infoE = Module['__ZTIN10__cxxabiv121__vmi_class_type_infoE'] = wasmExports['_ZTIN10__cxxabiv121__vmi_class_type_infoE'].value;
  __ZTSN10__cxxabiv121__vmi_class_type_infoE = Module['__ZTSN10__cxxabiv121__vmi_class_type_infoE'] = wasmExports['_ZTSN10__cxxabiv121__vmi_class_type_infoE'].value;
  __ZTVN10__cxxabiv117__pbase_type_infoE = Module['__ZTVN10__cxxabiv117__pbase_type_infoE'] = wasmExports['_ZTVN10__cxxabiv117__pbase_type_infoE'].value;
  __ZTVN10__cxxabiv129__pointer_to_member_type_infoE = Module['__ZTVN10__cxxabiv129__pointer_to_member_type_infoE'] = wasmExports['_ZTVN10__cxxabiv129__pointer_to_member_type_infoE'].value;
  __ZTVSt9bad_alloc = Module['__ZTVSt9bad_alloc'] = wasmExports['_ZTVSt9bad_alloc'].value;
  __ZTVSt20bad_array_new_length = Module['__ZTVSt20bad_array_new_length'] = wasmExports['_ZTVSt20bad_array_new_length'].value;
  __ZTISt9bad_alloc = Module['__ZTISt9bad_alloc'] = wasmExports['_ZTISt9bad_alloc'].value;
  __ZTISt20bad_array_new_length = Module['__ZTISt20bad_array_new_length'] = wasmExports['_ZTISt20bad_array_new_length'].value;
  __ZTVSt9exception = Module['__ZTVSt9exception'] = wasmExports['_ZTVSt9exception'].value;
  __ZTSSt9exception = Module['__ZTSSt9exception'] = wasmExports['_ZTSSt9exception'].value;
  __ZTVSt13bad_exception = Module['__ZTVSt13bad_exception'] = wasmExports['_ZTVSt13bad_exception'].value;
  __ZTISt13bad_exception = Module['__ZTISt13bad_exception'] = wasmExports['_ZTISt13bad_exception'].value;
  __ZTSSt13bad_exception = Module['__ZTSSt13bad_exception'] = wasmExports['_ZTSSt13bad_exception'].value;
  __ZTSSt9bad_alloc = Module['__ZTSSt9bad_alloc'] = wasmExports['_ZTSSt9bad_alloc'].value;
  __ZTSSt20bad_array_new_length = Module['__ZTSSt20bad_array_new_length'] = wasmExports['_ZTSSt20bad_array_new_length'].value;
  __ZTVSt12domain_error = Module['__ZTVSt12domain_error'] = wasmExports['_ZTVSt12domain_error'].value;
  __ZTISt12domain_error = Module['__ZTISt12domain_error'] = wasmExports['_ZTISt12domain_error'].value;
  __ZTSSt12domain_error = Module['__ZTSSt12domain_error'] = wasmExports['_ZTSSt12domain_error'].value;
  __ZTSSt11logic_error = Module['__ZTSSt11logic_error'] = wasmExports['_ZTSSt11logic_error'].value;
  __ZTVSt16invalid_argument = Module['__ZTVSt16invalid_argument'] = wasmExports['_ZTVSt16invalid_argument'].value;
  __ZTISt16invalid_argument = Module['__ZTISt16invalid_argument'] = wasmExports['_ZTISt16invalid_argument'].value;
  __ZTSSt16invalid_argument = Module['__ZTSSt16invalid_argument'] = wasmExports['_ZTSSt16invalid_argument'].value;
  __ZTVSt12length_error = Module['__ZTVSt12length_error'] = wasmExports['_ZTVSt12length_error'].value;
  __ZTISt12length_error = Module['__ZTISt12length_error'] = wasmExports['_ZTISt12length_error'].value;
  __ZTSSt12length_error = Module['__ZTSSt12length_error'] = wasmExports['_ZTSSt12length_error'].value;
  __ZTVSt12out_of_range = Module['__ZTVSt12out_of_range'] = wasmExports['_ZTVSt12out_of_range'].value;
  __ZTISt12out_of_range = Module['__ZTISt12out_of_range'] = wasmExports['_ZTISt12out_of_range'].value;
  __ZTSSt12out_of_range = Module['__ZTSSt12out_of_range'] = wasmExports['_ZTSSt12out_of_range'].value;
  __ZTVSt11range_error = Module['__ZTVSt11range_error'] = wasmExports['_ZTVSt11range_error'].value;
  __ZTISt11range_error = Module['__ZTISt11range_error'] = wasmExports['_ZTISt11range_error'].value;
  __ZTSSt11range_error = Module['__ZTSSt11range_error'] = wasmExports['_ZTSSt11range_error'].value;
  __ZTSSt13runtime_error = Module['__ZTSSt13runtime_error'] = wasmExports['_ZTSSt13runtime_error'].value;
  __ZTVSt14overflow_error = Module['__ZTVSt14overflow_error'] = wasmExports['_ZTVSt14overflow_error'].value;
  __ZTISt14overflow_error = Module['__ZTISt14overflow_error'] = wasmExports['_ZTISt14overflow_error'].value;
  __ZTSSt14overflow_error = Module['__ZTSSt14overflow_error'] = wasmExports['_ZTSSt14overflow_error'].value;
  __ZTVSt15underflow_error = Module['__ZTVSt15underflow_error'] = wasmExports['_ZTVSt15underflow_error'].value;
  __ZTISt15underflow_error = Module['__ZTISt15underflow_error'] = wasmExports['_ZTISt15underflow_error'].value;
  __ZTSSt15underflow_error = Module['__ZTSSt15underflow_error'] = wasmExports['_ZTSSt15underflow_error'].value;
  __ZTVSt8bad_cast = Module['__ZTVSt8bad_cast'] = wasmExports['_ZTVSt8bad_cast'].value;
  __ZTVSt10bad_typeid = Module['__ZTVSt10bad_typeid'] = wasmExports['_ZTVSt10bad_typeid'].value;
  __ZTISt10bad_typeid = Module['__ZTISt10bad_typeid'] = wasmExports['_ZTISt10bad_typeid'].value;
  __ZTVSt9type_info = Module['__ZTVSt9type_info'] = wasmExports['_ZTVSt9type_info'].value;
  __ZTSSt9type_info = Module['__ZTSSt9type_info'] = wasmExports['_ZTSSt9type_info'].value;
  __ZTSSt8bad_cast = Module['__ZTSSt8bad_cast'] = wasmExports['_ZTSSt8bad_cast'].value;
  __ZTSSt10bad_typeid = Module['__ZTSSt10bad_typeid'] = wasmExports['_ZTSSt10bad_typeid'].value;
}

var wasmImports = {
  /** @export */
  __call_sighandler: ___call_sighandler,
  /** @export */
  __syscall_fcntl64: ___syscall_fcntl64,
  /** @export */
  __syscall_ftruncate64: ___syscall_ftruncate64,
  /** @export */
  __syscall_ioctl: ___syscall_ioctl,
  /** @export */
  __syscall_mkdirat: ___syscall_mkdirat,
  /** @export */
  __syscall_openat: ___syscall_openat,
  /** @export */
  _abort_js: __abort_js,
  /** @export */
  _emscripten_get_progname: __emscripten_get_progname,
  /** @export */
  _emscripten_runtime_keepalive_clear: __emscripten_runtime_keepalive_clear,
  /** @export */
  _mmap_js: __mmap_js,
  /** @export */
  _munmap_js: __munmap_js,
  /** @export */
  _setitimer_js: __setitimer_js,
  /** @export */
  _tzset_js: __tzset_js,
  /** @export */
  emscripten_resize_heap: _emscripten_resize_heap,
  /** @export */
  environ_get: _environ_get,
  /** @export */
  environ_sizes_get: _environ_sizes_get,
  /** @export */
  fd_close: _fd_close,
  /** @export */
  fd_read: _fd_read,
  /** @export */
  fd_seek: _fd_seek,
  /** @export */
  fd_write: _fd_write,
  /** @export */
  proc_exit: _proc_exit
};


// include: postamble.js
// === Auto-generated postamble setup entry stuff ===

function run() {

  if (runDependencies > 0) {
    dependenciesFulfilled = run;
    return;
  }

  preRun();

  // a preRun added a dependency, run will be called later
  if (runDependencies > 0) {
    dependenciesFulfilled = run;
    return;
  }

  function doRun() {
    // run may have just been called through dependencies being fulfilled just in this very frame,
    // or while the async setStatus time below was happening
    Module['calledRun'] = true;

    if (ABORT) return;

    initRuntime();

    Module['onRuntimeInitialized']?.();

    postRun();
  }

  if (Module['setStatus']) {
    Module['setStatus']('Running...');
    setTimeout(() => {
      setTimeout(() => Module['setStatus'](''), 1);
      doRun();
    }, 1);
  } else
  {
    doRun();
  }
}

var wasmExports;

// With async instantation wasmExports is assigned asynchronously when the
// instance is received.
createWasm();

run();

// end include: postamble.js

