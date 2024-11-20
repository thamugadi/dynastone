# dynastone
This project uses Keystone Assembler to assemble instructions by leaving "holes" in certain operands, which are to be replaced by a variable specified along with its size using a special syntax. At the end, a C code is generated, intended to contain calls to an emitter, such as those commonly used in JIT compilers.
It works in a similar way to a dynamic assembler like LuaJIT's dynasm.

Usage:
``dynastone <arch> <code> <emit_function_8> <emit_function_16> <emit_function_32> <emit_function_64>``

Example:

```
$ ./dynastone "x64" "mov rdi, |var1,64|" "emit_8"

emit_8(0x48);
emit_8(0xbf);
emit_64(var1);
```

```
$ ./dynastone "ppc64be" "addi |var1,5|,|var2,5|,|var3, 16|" "emit_8" "emit_16"

emit_8(0x38 | (var1 & 0x03));
emit_8(0x00 | (((var1 >> 2) & 0x07) << 5) | (var2 & 0x1f));
emit_16(var3);
```

The user must specify the maximum operand size, in bits. If there is a minimum size below which the instruction structure changes, it is the user's responsibility to ensure the specified variable does not fall below this size.

An instruction with multiple immediate operands where the order of their occurrence in the generated machine code is different from the order of their occurrence in the mnemonic instruction, unless its architecture has a big-endian equivalent supported by keystone where the order matches, will not be supported.

**In its current state, it will not work for instructions where the mnemonic syntax presents the instruction as having one operand, but where it is discontinuous in the assembled code, such as ``mov reg, #imm`` with ``imm`` > 255 in ARMv6T2 and later.**

# Integration with text editors
An interesting application of this project could be integration with text editors to directly generate calls to emitters, based on instructions written with placeholders in the syntax of the tool, which would be written in comments.

An integration with emacs could take this form:

```elisp
(defvar *dynastone-bin* "{path to dynastone binary file}")
(defvar *emit-8* "emit_8") (defvar *emit-16* "emit_16") (defvar *emit-32* "emit_32") (defvar *emit-64* "emit_64") 
(defvar *arch* "x64")

(defun append-f-comment-to-lines (f)
  (when (use-region-p)
    (save-excursion
      (save-restriction
        (narrow-to-region (region-beginning) (region-end))
        (goto-char (point-min))
        (while (re-search-forward "^\\([ \t]*\\)//[ \t]*\\(.*\\)$" nil t)
          (let* ((indent (match-string 1))
                 (comment (match-string 2))
                 (result-lines (split-string (funcall f comment) "\n" t))
                 (last-index (1- (length result-lines))))
            (end-of-line)
            (newline)
            (dolist (line result-lines)
              (insert indent line)
              (unless (eq (cl-position line result-lines) last-index)
                (newline)))))))))

(defun send-to-dynastone (comment-text)
  (let* ((args (list *arch* comment-text *emit-8* *emit-16* *emit-32* *emit-64*))
         (output-buffer (generate-new-buffer "*dynastone-output*")))
    (unwind-protect
        (progn
          (apply 'call-process *dynastone-bin* nil output-buffer nil args)
          (with-current-buffer output-buffer
            (string-trim (buffer-string))))
      (kill-buffer output-buffer))))

(defun run-dynastone ()
  (interactive)
  (append-f-comment-to-lines 'send-to-dynastone))
(global-set-key (kbd "C-c f") 'run-dynastone)
```
A demonstration:
![](https://i.imgur.com/uBcNsQO.gif)
