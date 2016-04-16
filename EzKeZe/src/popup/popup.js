$(document).ready(function() {
    var chat_key; // the AES key for the current chat

    $("#encrypt").click(function() {
        plaintext = getActiveText();
        ciphertext = encrypt(plaintext, chat_key);
        replaceActiveText(ciphertext);
    });
});

/**
 * grabs the current text in active textfield
 * @return: a string representing the text
 */
function getActiveText() {
    return $(':focus').val();
}

/**
 * takes a plaintext and encrypts it based of the active chat
 * @param: plaintext the plaintext string to encrypt
 * @param: current_chat_key the AES key for the currently active chat (chosen by user from dropdown)
 * @return: the AES encrypted ciphertext
 */
function encrypt(plaintext, current_chat_key) {
}

/**
 * finds the currently active text area dn replaces the current text
 * @param: replacement_text the text that will replace whatever is in the text area
 */
function replaceActiveText(replacement_text) {
}
