const CaptchaWidget = (() => {
    let captchaId = null;
    let container = null;
    let config = { apiUrl: '' };

    function render() {
        container.innerHTML = `
            <div class="captcha-box">
                <div class="captcha-media">
                    <img class="captcha-img" alt="CAPTCHA">
                    <button type="button" class="captcha-audio" title="Ouvir">🔊</button>
                    <button type="button" class="captcha-refresh" title="Novo">🔄</button>
                </div>
                <input type="text" class="captcha-input"
                       placeholder="Digite o código" autocomplete="off">
            </div>`;
        load();
        bind();
    }
    async function load() {
        const res = await fetch(config.apiUrl + '/api/captcha');
        const data = await res.json();
        captchaId = data.captcha_id;
        container.querySelector('.captcha-img').src =
            config.apiUrl + data.image_url;
        container.querySelector('.captcha-audio').dataset.url =
            config.apiUrl + data.audio_url;
    }
    function bind() {
        container.querySelector('.captcha-refresh')
            .onclick = load;
        container.querySelector('.captcha-audio').onclick = (e) => {
            new Audio(e.target.dataset.url).play();
        };
    }
    return {
        mount(selector, opts = {}) {
            container = document.querySelector(selector);
            Object.assign(config, opts);
            render();
        },
        async getToken() {
            const answer = container.querySelector('.captcha-input').value;
            if (!answer || !captchaId) return null;

            const res = await fetch(
                `config.apiUrl/api/captcha/{config.apiUrl}/api/captcha/config.apiUrl/api/captcha/{captchaId}/verify`, {
                    method: 'POST',
                    headers: {'Content-Type': 'application/json'},
                    body: JSON.stringify({ answer })
                });
            const data = await res.json();

            if (data.verified) return data.token;
            load(); 
            return null;
        }
    };
})();
