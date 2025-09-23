class LumasonicPlayer {
    constructor() {
        // Extract host and port from URL parameters, or auto-detect from current location
        const urlParams = new URLSearchParams(window.location.search);
        const host = urlParams.get('host') || window.location.hostname || 'localhost';
        const port = urlParams.get('port') || window.location.port || '8080';
        
        // Build the API base URL dynamically
        this.apiBase = `http://${host}:${port}/api/v1`;
        
        // Log the detected connection details for debugging
        console.log(`LumasonicPlayer: Connecting to ${this.apiBase}`);
        console.log(`LumasonicPlayer: Detected host: ${host}, port: ${port}`);
        
        this.updateInterval = null;
        this.isDragging = false;
        this.isVolumeDragging = false; // Track volume slider dragging
        this.isProgressDragging = false; // Track progress bar dragging
        this.isBrightnessDragging = false; // Track brightness slider dragging
        this.isStopPending = false; // Track when stop is pressed to prevent time jumping
        this.ignoreNextTimeUpdate = false; // Ignore next polling time update after seek
        this.isConnected = true; // Track connection state to prevent duplicate notifications
        this.currentFile = null;
        this.isPlaying = false;
        this.isLooping = false;
        this.currentTime = 0;
        this.totalTime = 0;
        this.volume = 1.0;
        this.brightness = 1.0;
        
        // Connection notification debouncing
        this.lastConnectionNotification = 0;
        this.lastDisconnectionNotification = 0;
        this.connectionNotificationTimeout = 2000; // 2 second timeout
        
        // Polling error logging debouncing
        this.lastPollingErrorLog = 0;
        this.pollingErrorLogTimeout = 10000; // 10 second timeout for polling error logs
        
        // Playlist state
        this.hasPlaylist = false;
        this.playlistFinished = false;
        this.playlistItems = [];
        
        this.initializeElements();
        this.bindEvents();
        this.startPolling(); // Re-enabled
        this.updateUI();
    }
    
    initializeElements() {
        // Transport controls
        this.playBtn = document.getElementById('playBtn');
        this.pauseBtn = document.getElementById('pauseBtn');
        this.stopBtn = document.getElementById('stopBtn');
        this.prevBtn = document.getElementById('prevBtn');
        this.nextBtn = document.getElementById('nextBtn');
        
        // Progress controls
        this.progressBar = document.getElementById('progressBar');
        this.progressFill = document.getElementById('progressFill');
        this.progressHandle = document.getElementById('progressHandle');
        this.currentTimeDisplay = document.getElementById('currentTime');
        this.totalTimeDisplay = document.getElementById('totalTime');
        
        // Volume controls
        this.volumeSlider = document.getElementById('gainSlider');
        this.volumeValue = document.getElementById('gainValue');
        this.volumeIcon = document.getElementById('gainIcon');
        
        // Brightness controls
        this.brightnessSlider = document.getElementById('brightnessSlider');
        this.brightnessValue = document.getElementById('brightnessValue');
        this.brightnessIcon = document.getElementById('brightnessIcon');
        
        // File info
        this.fileName = document.getElementById('fileName');
        
        // Connection status
        this.connectionStatus = document.getElementById('connectionStatus');
        
        // Playlist info
        this.playlistInfo = document.getElementById('playlistInfo');
        this.playlistCount = document.getElementById('playlistCount');
        this.playlistItemsContainer = document.getElementById('playlistItems');
        
        // Loop control
        this.loopBtn = document.getElementById('loopBtn');
        
        // Notification area
        this.notificationArea = document.getElementById('notificationArea');
    }
    
    bindEvents() {
        // Transport controls
        this.playBtn.addEventListener('click', () => this.play());
        this.pauseBtn.addEventListener('click', () => this.pause());
        this.stopBtn.addEventListener('click', () => this.stop());
        
        // Progress bar - re-enabled with dragging protection
        this.progressBar.addEventListener('click', (e) => this.handleProgressClick(e));
        this.progressHandle.addEventListener('mousedown', (e) => this.startProgressDragging(e));
        document.addEventListener('mousemove', (e) => this.handleProgressDragging(e));
        document.addEventListener('mouseup', () => this.stopProgressDragging());
        
        // Touch support for progress bar - re-enabled with dragging protection
        this.progressHandle.addEventListener('touchstart', (e) => this.startProgressDragging(e));
        document.addEventListener('touchmove', (e) => this.handleProgressDragging(e));
        document.addEventListener('touchend', () => this.stopProgressDragging());
        
        // Volume control
        this.volumeSlider.addEventListener('input', (e) => {
            this.setVolume(e.target.value / 100);
        });
        this.volumeSlider.addEventListener('mousedown', () => { 
            this.isVolumeDragging = true; 
        });
        this.volumeSlider.addEventListener('mouseup', () => { 
            this.isVolumeDragging = false; 
        });
        this.volumeSlider.addEventListener('touchstart', () => { 
            this.isVolumeDragging = true; 
        });
        this.volumeSlider.addEventListener('touchend', () => { 
            this.isVolumeDragging = false; 
        });
        
        // Brightness control
        this.brightnessSlider.addEventListener('input', (e) => {
            this.setBrightness(e.target.value / 100);
        });
        this.brightnessSlider.addEventListener('mousedown', () => { 
            this.isBrightnessDragging = true; 
        });
        this.brightnessSlider.addEventListener('mouseup', () => { 
            this.isBrightnessDragging = false; 
        });
        this.brightnessSlider.addEventListener('touchstart', () => { 
            this.isBrightnessDragging = true; 
        });
        this.brightnessSlider.addEventListener('touchend', () => { 
            this.isBrightnessDragging = false; 
        });
        
        // Global listeners to catch mouseup/touchend outside the slider
        document.addEventListener('mouseup', () => {
            if (this.isVolumeDragging) {
                this.isVolumeDragging = false;
            }
            if (this.isBrightnessDragging) {
                this.isBrightnessDragging = false;
            }
            if (this.isProgressDragging) {
                this.stopProgressDragging();
            }
        });
        document.addEventListener('touchend', () => {
            if (this.isVolumeDragging) {
                this.isVolumeDragging = false;
            }
            if (this.isBrightnessDragging) {
                this.isBrightnessDragging = false;
            }
            if (this.isProgressDragging) {
                this.stopProgressDragging();
            }
        });
        
        // Loop control
        this.loopBtn.addEventListener('click', () => this.setLooping(!this.isLooping));
        
        // Playlist navigation
        this.prevBtn.addEventListener('click', () => this.loadPreviousTrack());
        this.nextBtn.addEventListener('click', () => this.loadNextTrack());
    }
    
    async apiCall(endpoint, method = 'GET', data = null, isPolling = false) {
        try {
            const options = {
                method,
                headers: {
                    'Content-Type': 'application/json',
                }
            };
            
            if (data) {
                options.body = JSON.stringify(data);
            }
            
            const response = await fetch(`${this.apiBase}${endpoint}`, options);
            
            if (!response.ok) {
                throw new Error(`HTTP error! status: ${response.status}`);
            }
            
            // If we get here, we're connected
            if (!this.isConnected) {
                console.log('Player connection restored');
                this.isConnected = true;
                this.hideConnectionStatus();
                
                // Only show reconnection notification if enough time has passed
                const now = Date.now();
                if (now - this.lastConnectionNotification > this.connectionNotificationTimeout) {
                    this.updateStatus('Server connection restored', 'success');
                    this.lastConnectionNotification = now;
                    // Reset disconnection timestamp to prevent immediate "lost" notification
                    this.lastDisconnectionNotification = now;
                }
            }
            
            return await response.json();
        } catch (error) {
            // Check if we're transitioning from connected to disconnected
            if (this.isConnected) {
                console.log('Connection lost detected:', error.message);
                this.isConnected = false;
                this.showConnectionStatus();
                
                // Only show disconnection notification if enough time has passed
                const now = Date.now();
                if (now - this.lastDisconnectionNotification > this.connectionNotificationTimeout) {
                    this.updateStatus(`Server connection lost`, 'error');
                    this.lastDisconnectionNotification = now;
                }
            }
            
            // Log errors to console with different behavior for polling vs non-polling calls
            if (isPolling) {
                // For polling calls, only log errors once every 10 seconds
                const now = Date.now();
                if (now - this.lastPollingErrorLog > this.pollingErrorLogTimeout) {
                    console.error('Polling API call failed:', error);
                    this.lastPollingErrorLog = now;
                }
            } else {
                // For non-polling calls, log errors every time
            console.error('API call failed:', error);
            }
            
            return null;
        }
    }
    
    async play() {
        const result = await this.apiCall('/transport/play', 'POST');
        if (result && result.success) {
            this.isPlaying = true;
            this.isStopPending = false; // Clear stop pending flag when play is pressed
            this.updateUI();
            this.updateStatus('Playing');
        }
    }
    
    async pause() {
        const result = await this.apiCall('/transport/pause', 'POST');
        if (result && result.success) {
            this.isPlaying = false;
            this.isStopPending = false; // Clear stop pending flag when pause is pressed
            this.updateUI();
            this.updateStatus('Paused');
        }
    }
    
    async stop() {
        const result = await this.apiCall('/transport/stop', 'POST');
        if (result && result.success) {
            this.isPlaying = false;
            this.currentTime = 0;
            this.isStopPending = true; // Set flag to prevent polling from updating time
            this.updateUI();
            this.updateStatus('Stopped');
        }
    }
    
    async setVolume(volume) {
        this.volume = volume;
        const result = await this.apiCall('/gain', 'POST', { gain: volume });
        if (result && result.success) {
            this.updateVolumeUI();
            this.updateStatus(`Volume: ${Math.round(volume * 100)}%`);
        }
    }
    
    async setBrightness(brightness) {
        this.brightness = brightness;
        const result = await this.apiCall('/brightness', 'POST', { brightness: brightness });
        if (result && result.success) {
            this.updateBrightnessUI();
            this.updateStatus(`Brightness: ${Math.round(brightness * 100)}%`);
        }
    }
    
    async setLooping(looping) {
        this.isLooping = looping;
        const result = await this.apiCall('/stream/loop', 'POST', { loop: looping });
        if (result && result.success) {
            this.updateStatus(looping ? 'Loop enabled' : 'Loop disabled');
        }
    }
    
    async loadPlaylistItem(index) {
        // Check if the selected item is the same as the currently playing file
        const selectedItem = this.playlistItems[index];
        if (!selectedItem) return;
        
        // If the selected item is the same as the current file, don't reload it
        if (this.currentFile && selectedItem === this.currentFile) {
            // Just update the UI to highlight the current item without making an API call
            this.updatePlaylistUI();
            return;
        }
        
        // Optimistic UI update - immediately update the UI as if the request succeeded
        this.currentFile = selectedItem;
        this.fileName.textContent = selectedItem;
        this.adjustFileNameFontSize();
        
        // Update playlist UI to highlight the selected item
        this.updatePlaylistUI();
        
        // Update transport buttons state
        this.playBtn.disabled = false;
        this.pauseBtn.disabled = true; // Will be playing after load
        this.stopBtn.disabled = false;
        
        // Make the API call
        const result = await this.apiCall('/load/playlist/item', 'POST', { index: index });
        if (result && result.success) {
            this.updateStatus(`Loaded playlist item ${index + 1}`);
        } else {
            // Revert optimistic update on error
            this.updateStatus('Failed to load playlist item', 'error');
            
            // The next polling cycle will restore the correct state from the server
            // For now, we could optionally revert the UI changes here if needed
        }
    }
    
    async loadPreviousTrack() {
        if (!this.hasPlaylist || this.playlistItems.length === 0) return;
        
        // Find current index
        const currentIndex = this.playlistItems.findIndex(item => item === this.currentFile);
        if (currentIndex === -1 || currentIndex === 0) return; // Don't go previous if at first track
        
        // Calculate previous index (no wrap-around)
        const prevIndex = currentIndex - 1;
        await this.loadPlaylistItem(prevIndex);
    }
    
    async loadNextTrack() {
        if (!this.hasPlaylist || this.playlistItems.length === 0) return;
        
        // Find current index
        const currentIndex = this.playlistItems.findIndex(item => item === this.currentFile);
        if (currentIndex === -1 || currentIndex >= this.playlistItems.length - 1) return; // Don't go next if at last track
        
        // Calculate next index (no wrap-around)
        const nextIndex = currentIndex + 1;
        await this.loadPlaylistItem(nextIndex);
    }
    
    async seekToTime(time, unit = 'sec') {
        // Set flag to ignore next polling time update
        this.ignoreNextTimeUpdate = true;
        
        const data = unit === 'sec' ? { unit: 'sec', time: time } : { unit: '%', time: time };
        const result = await this.apiCall('/transport/time', 'POST', data);
        if (result && result.success) {
            this.currentTime = time;
            this.updateProgressUI();
        }
    }
    
    handleProgressClick(e) {
        if (this.isDragging) return;
        
        const rect = this.progressBar.getBoundingClientRect();
        const clickX = e.clientX - rect.left;
        const percentage = clickX / rect.width;
        
        if (this.totalTime > 0) {
            const newTime = percentage * this.totalTime;
            this.seekToTime(newTime);
        }
    }
    
    startProgressDragging(e) {
        e.preventDefault();
        this.isProgressDragging = true;
        this.progressHandle.style.cursor = 'grabbing';
    }
    
    handleProgressDragging(e) {
        if (!this.isProgressDragging) return;
        
        e.preventDefault();
        const rect = this.progressBar.getBoundingClientRect();
        const clientX = e.clientX || (e.touches && e.touches[0] ? e.touches[0].clientX : 0);
        const clickX = Math.max(0, Math.min(clientX - rect.left, rect.width));
        const percentage = clickX / rect.width;
        
        this.updateProgressVisual(percentage);
        
        if (this.totalTime > 0) {
            const newTime = percentage * this.totalTime;
            this.currentTime = newTime;
            this.updateTimeDisplay();
        }
    }
    
    stopProgressDragging() {
        if (this.isProgressDragging) {
            this.isProgressDragging = false;
            this.progressHandle.style.cursor = 'grab';
            
            if (this.totalTime > 0) {
                this.seekToTime(this.currentTime);
            }
        }
    }
    
    updateProgressVisual(percentage) {
        const clampedPercentage = Math.max(0, Math.min(1, percentage));
        this.progressFill.style.width = `${clampedPercentage * 100}%`;
        this.progressHandle.style.left = `${clampedPercentage * 100}%`;
    }
    
    updateUI() {
        // Update transport buttons
        this.playBtn.disabled = !this.currentFile || this.isPlaying;
        this.pauseBtn.disabled = !this.currentFile || !this.isPlaying;
        this.stopBtn.disabled = !this.currentFile;
        
        // Update progress
        this.updateProgressUI();
        
        // Update volume
        this.updateVolumeUI();
        
        // Update brightness
        this.updateBrightnessUI();
        
        // Update loop toggle
        if (this.isLooping) {
            this.loopBtn.classList.add('active');
        } else {
            this.loopBtn.classList.remove('active');
        }
        
        // Update playlist UI
        this.updatePlaylistUI();
        
        // Adjust font size for file name
        this.adjustFileNameFontSize();
    }
    
    updateProgressUI() {
        if (this.totalTime > 0) {
            const percentage = this.currentTime / this.totalTime;
            this.updateProgressVisual(percentage);
            this.updateTimeDisplay();
        } else {
            this.updateProgressVisual(0);
            this.currentTimeDisplay.textContent = '0:00';
            this.totalTimeDisplay.textContent = '0:00';
        }
    }
    
    updateTimeDisplay() {
        this.currentTimeDisplay.textContent = this.formatTime(this.currentTime);
        this.totalTimeDisplay.textContent = this.formatTime(this.totalTime);
    }
    
    updateVolumeUI() {
        const volumePercent = Math.round(this.volume * 100);
        this.volumeSlider.value = volumePercent;
        this.volumeValue.textContent = `${volumePercent}%`;
        
        // Update volume icon based on level
        if (volumePercent === 0) {
            this.volumeIcon.innerHTML = '<path d="M16.5 12c0-1.77-1.02-3.29-2.5-4.03v2.21l2.45 2.45c.03-.2.05-.41.05-.63zm2.5 0c0 .94-.2 1.82-.54 2.64l1.51 1.51C20.63 14.91 21 13.5 21 12c0-4.28-2.99-7.86-7-8.77v2.06c2.89.86 5 3.54 5 6.71zM4.27 3L3 4.27 7.73 9H3v6h4l5 5v-6.73l4.25 4.25c-.67.52-1.42.93-2.25 1.18v2.06c1.38-.31 2.63-.95 3.69-1.81L19.73 21 21 19.73l-9-9L4.27 3zM12 4L9.91 6.09 12 8.18V4z"/>';
        } else if (volumePercent < 50) {
            this.volumeIcon.innerHTML = '<path d="M18.5 12c0-1.77-1.02-3.29-2.5-4.03v8.05c1.48-.73 2.5-2.25 2.5-4.02zM5 9v6h4l5 5V4L9 9H5z"/>';
        } else {
            this.volumeIcon.innerHTML = '<path d="M3 9v6h4l5 5V4L7 9H3zm13.5 3c0-1.77-1.02-3.29-2.5-4.03v8.05c1.48-.73 2.5-2.25 2.5-4.02zM14 3.23v2.06c2.89.86 5 3.54 5 6.71s-2.11 5.85-5 6.71v2.06c4.01-.91 7-4.49 7-8.77s-2.99-7.86-7-8.77z"/>';
        }
    }
    
    updateBrightnessUI() {
        const brightnessPercent = Math.round(this.brightness * 100);
        this.brightnessSlider.value = brightnessPercent;
        this.brightnessValue.textContent = `${brightnessPercent}%`;
        
        // Keep the same sun icon for all brightness levels
        // The icon color changes based on the CSS class
    }
    
    formatTime(seconds) {
        if (!seconds || seconds < 0) return '0:00';
        
        const minutes = Math.floor(seconds / 60);
        const remainingSeconds = Math.floor(seconds % 60);
        return `${minutes}:${remainingSeconds.toString().padStart(2, '0')}`;
    }
    
    updateStatus(message, type = 'info') {
        // Only show notifications for errors and success (connection status changes)
        if (type !== 'error' && type !== 'success') {
            return;
        }
        
        // Create notification element
        const notification = document.createElement('div');
        notification.className = `notification ${type}`;
        
        // Create icon based on type
        const icon = document.createElement('div');
        icon.className = 'notification-icon';
        
        if (type === 'error') {
            icon.innerHTML = `
                <svg viewBox="0 0 24 24" fill="currentColor">
                    <path d="M12 2C6.48 2 2 6.48 2 12s4.48 10 10 10 10-4.48 10-10S17.52 2 12 2zm1 15h-2v-2h2v2zm0-4h-2V7h2v6z"/>
                </svg>
            `;
        } else if (type === 'success') {
            icon.innerHTML = `
                <svg viewBox="0 0 24 24" fill="currentColor">
                    <path d="M9 16.17L4.83 12l-1.42 1.41L9 19 21 7l-1.41-1.41z"/>
                </svg>
            `;
        }
        
        // Create message
        const messageDiv = document.createElement('div');
        messageDiv.className = 'notification-message';
        messageDiv.textContent = message;
        
        // Create close button
        const closeBtn = document.createElement('button');
        closeBtn.className = 'notification-close';
        closeBtn.innerHTML = `
            <svg viewBox="0 0 24 24" fill="currentColor">
                <path d="M19 6.41L17.59 5 12 10.59 6.41 5 5 6.41 10.59 12 5 17.59 6.41 19 12 13.41 17.59 19 19 17.59 13.41 12z"/>
            </svg>
        `;
        closeBtn.addEventListener('click', () => {
            notification.classList.remove('show');
            setTimeout(() => {
                if (notification.parentNode) {
                    notification.parentNode.removeChild(notification);
                }
            }, 300);
        });
        
        // Assemble notification
        notification.appendChild(icon);
        notification.appendChild(messageDiv);
        notification.appendChild(closeBtn);
        
        // Add to notification area
        this.notificationArea.appendChild(notification);
        
        // Show notification with animation
        setTimeout(() => {
            notification.classList.add('show');
        }, 10);
        
        // Auto-remove after 5 seconds
        setTimeout(() => {
            if (notification.parentNode) {
                notification.classList.remove('show');
                setTimeout(() => {
                    if (notification.parentNode) {
                        notification.parentNode.removeChild(notification);
                    }
                }, 300);
            }
        }, 5000);
    }
    
    showConnectionStatus() {
        if (this.connectionStatus) {
            this.connectionStatus.style.display = 'flex';
        }
        if (this.fileName) {
            this.fileName.style.display = 'none';
        }
    }
    
    hideConnectionStatus() {
        if (this.connectionStatus) {
            this.connectionStatus.style.display = 'none';
        }
        if (this.fileName) {
            this.fileName.style.display = '';
        }
    }
    
    startPolling() {
        // Initial state update
        this.updatePlayerState();
        
        // Poll for updates every 500ms
        this.updateInterval = setInterval(() => {
            this.updatePlayerState();
        }, 500);
    }
    
    stopPolling() {
        if (this.updateInterval) {
            clearInterval(this.updateInterval);
            this.updateInterval = null;
        }
    }
    
    async updatePlayerState() {
        // Get complete player state from single API call
        const state = await this.apiCall('/state', 'POST', null, true); // Mark as polling call
        if (state && state.success) {
            // Update all state variables from the response
            // Only update time if not currently dragging the progress bar
            if (!this.isProgressDragging) {
                const serverTime = state.timeSec || 0;
                
                // If stop is pending, only update time when server time reaches 0
                if (this.isStopPending) {
                    if (serverTime === 0) {
                        this.currentTime = 0;
                        this.isStopPending = false; // Clear the flag
                    }
                    // Don't update time from server while stop is pending
                } else if (this.ignoreNextTimeUpdate) {
                    // Ignore this polling cycle's time update after a seek operation
                    this.ignoreNextTimeUpdate = false;
                } else {
                    this.currentTime = serverTime;
                }
            }
            this.totalTime = state.lengthSec || 0;
            this.isPlaying = state.playing || false;
            this.isLooping = state.looping || false;
            this.currentFile = state.fileName || null;
            
            // Only update volume from polling if not currently dragging the volume slider
            if (!this.isVolumeDragging) {
                this.volume = (state.gain !== undefined && state.gain !== null) ? state.gain : 1.0;
            }
            
            // Only update brightness from polling if not currently dragging the brightness slider
            if (!this.isBrightnessDragging) {
                this.brightness = (state.brightness !== undefined && state.brightness !== null) ? state.brightness : 1.0;
            }
            
            // Handle playlist state from the main state response
            if (state.playlist) {
                // Playlist is active
                const hadPlaylist = this.hasPlaylist;
                this.hasPlaylist = true;
                this.playlistFinished = state.playlist.isFinished || false;
                
                // Only fetch playlist items if we just discovered a playlist (not polling)
                if (!hadPlaylist) {
                    const playlistItems = await this.apiCall('/playlist/items', 'POST', null, true); // Mark as polling call
                    if (playlistItems && playlistItems.success) {
                        this.playlistItems = playlistItems.items || [];
                    }
                }
            } else {
                // No playlist is active
                if (this.hasPlaylist) {
                    this.hasPlaylist = false;
                    this.playlistFinished = false;
                    this.playlistItems = [];
                }
            }
            
            // Update UI elements directly
            const newFileName = this.currentFile || 'No file loaded';
            if (this.fileName.textContent !== newFileName) {
                this.fileName.textContent = newFileName;
            this.adjustFileNameFontSize();
            }
            
            if (this.isLooping) {
                this.loopBtn.classList.add('active');
            } else {
                this.loopBtn.classList.remove('active');
            }
            
            // Update progress and time display
            this.updateProgressUI();
            
            // Update volume UI
            this.updateVolumeUI();
            
            // Update brightness UI
            this.updateBrightnessUI();
        }
        
        this.updateUI();
    }
    
    updatePlaylistUI() {
        if (this.hasPlaylist && this.playlistItems.length > 0) {
            // Show playlist info
            this.playlistInfo.style.display = 'block';
            
            // Show navigation buttons
            this.prevBtn.style.display = 'flex';
            this.nextBtn.style.display = 'flex';
            
            // Find current index to determine button states
            const currentIndex = this.playlistItems.findIndex(item => item === this.currentFile);
            
            // Disable previous button if at first track
            this.prevBtn.disabled = currentIndex === -1 || currentIndex === 0;
            
            // Disable next button if at last track
            this.nextBtn.disabled = currentIndex === -1 || currentIndex >= this.playlistItems.length - 1;
            
            // Update playlist count
            this.playlistCount.textContent = `${this.playlistItems.length} track${this.playlistItems.length !== 1 ? 's' : ''}`;
            
            // Store the previous current file to detect changes
            const previousCurrentFile = this.playlistItemsContainer.querySelector('.playlist-item.current')?.textContent;
            
            // Clear and populate playlist items
            this.playlistItemsContainer.innerHTML = '';
            
            let currentItemElement = null;
            
            this.playlistItems.forEach((item, index) => {
                const itemElement = document.createElement('div');
                itemElement.className = 'playlist-item';
                itemElement.title = item; // Add tooltip with full filename
                
                // Check if this is the current file
                const isCurrent = this.currentFile && item === this.currentFile;
                if (isCurrent) {
                    itemElement.classList.add('current');
                    currentItemElement = itemElement;
                }
                
                // Add click handler to load this playlist item
                itemElement.addEventListener('click', (e) => {
                    e.preventDefault();
                    e.stopPropagation();
                    
                    // Prevent text selection
                    e.target.style.userSelect = 'none';
                    
                    // Add visual feedback
                    itemElement.style.opacity = '0.7';
                    setTimeout(() => {
                        itemElement.style.opacity = '1';
                    }, 150);
                    
                    this.loadPlaylistItem(index);
                });
                
                // Prevent text selection on mousedown
                itemElement.addEventListener('mousedown', (e) => {
                    e.preventDefault();
                });
                
                itemElement.textContent = item;
                this.playlistItemsContainer.appendChild(itemElement);
            });
            
            // Auto-scroll to current item if it changed
            if (currentItemElement && this.currentFile !== previousCurrentFile) {
                // Use scrollIntoView to smoothly scroll to the current item
                currentItemElement.scrollIntoView({
                    behavior: 'smooth',
                    block: 'nearest',
                    inline: 'nearest'
                });
            }
        } else {
            // Hide playlist info
            this.playlistInfo.style.display = 'none';
            
            // Hide navigation buttons
            this.prevBtn.style.display = 'none';
            this.nextBtn.style.display = 'none';
        }
    }
    
    adjustFileNameFontSize() {
        const fileName = this.fileName;
        const text = fileName.textContent;
        
        if (!text || text === 'No file loaded') {
            fileName.style.fontSize = 'clamp(14px, 4vw, 24px)';
            return;
        }
        
        // Get container width
        const containerWidth = fileName.parentElement.offsetWidth;
        const maxFontSize = Math.min(24, Math.max(14, containerWidth * 0.06)); // 6% of container width, clamped
        
        // Estimate text length and adjust font size
        const textLength = text.length;
        let fontSize;
        
        if (textLength <= 20) {
            fontSize = maxFontSize;
        } else if (textLength <= 40) {
            fontSize = maxFontSize * 0.85;
        } else if (textLength <= 60) {
            fontSize = maxFontSize * 0.7;
        } else {
            fontSize = maxFontSize * 0.6;
        }
        
        // Ensure minimum font size
        fontSize = Math.max(12, fontSize);
        
        fileName.style.fontSize = `${fontSize}px`;
    }
}

// Initialize the player when the page loads
document.addEventListener('DOMContentLoaded', () => {
    window.lumasonicPlayer = new LumasonicPlayer();
});

// Clean up when the page unloads
window.addEventListener('beforeunload', () => {
    if (window.lumasonicPlayer) {
        window.lumasonicPlayer.stopPolling();
    }
});
